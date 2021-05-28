//
// Created by Darwin Yuan on 2021/5/27.
//

#include <cub/array/ObjectArray.h>
#include <cstdint>

using MoId = uint8_t;

struct PhyCfgChangeState {
    constexpr static uint8_t PRESENT = 0x01;
    constexpr static uint8_t CHANGED = 0x02;

    constexpr static uint8_t DO_NOTHING = 0;
    constexpr static uint8_t RELEASE    = CHANGED;
    constexpr static uint8_t NO_CHANGE  = PRESENT;
    constexpr static uint8_t RE_CONFIG   = PRESENT | CHANGED;

    uint8_t selected : 1;
    uint8_t changed : 1;

    auto DoNothing() -> void {
        selected = false;
        changed = false;
    }

    auto ReCfg() -> void {
        selected = true;
        changed = true;
    }
};

struct CellGroup {
    static constexpr uint8_t SCG_ONLY = 0x00;
    static constexpr uint8_t MCG_ONLY = 0x01;
    static constexpr uint8_t SCG_MCG  = 0x02;
    static constexpr uint8_t MCG_SCG  = 0x03;

    uint8_t mcg:1;
    uint8_t shared:1;
};

///////////////////////////////////////////////
struct PhyMoCfgState {
    MoId            moId;
    CellGroup       cg;
    PhyCfgChangeState  freqChgState;
};

struct PhyServingMoCfgState : PhyMoCfgState {
    PhyCfgChangeState  cellChgState;
};

struct PhyRsMoCfgState {
    PhyServingMoCfgState pCell;
    PhyServingMoCfgState psCell;

    ObjectArray<PhyServingMoCfgState, 3> sCells;
    ObjectArray<PhyMoCfgState, 8> interFreq;
    bool shouldReleaseSCells{false};
    bool shouldReleaseInterFreq{false};
};

struct PhyMoCfgStateMsg {
    PhyRsMoCfgState ssb;
    PhyRsMoCfgState csi;
};

///////////////////////////////////////////////////////////////
struct SelectedMo {
    MoId    moId;
    CellGroup cg;

    auto IsCfgChanged(MoId moId) const -> bool {
        return (changed || moId != this->moId);
    }

    uint8_t changed:1;
    uint8_t shouldMeasureNeigh:1;
};

struct SelectedRsMoList {
    SelectedMo pCell;
    std::optional<SelectedMo> psCell;

    ObjectArray<SelectedMo, 3> sCells;
    ObjectArray<SelectedMo, 8> interFreq;
};

using SelectedMoList = ObjectArray<SelectedRsMoList, 2> ;

struct LocalCfgState {
    LocalCfgState() : configured{false}, confirmed{false} {}

    auto BuildSelected(PhyCfgChangeState& phy, MoId moId, SelectedMo const& selected) const -> void {
        phy.selected = true;
        phy.changed = selected.IsCfgChanged(moId) || NeverConfigured();
    }

    auto BuildNonSelected(PhyCfgChangeState& freq) const -> void {
        freq.selected = false;
        freq.changed = EverConfigured();
    }

    auto Build(PhyCfgChangeState& msg, SelectedMo const& selected, MoId moId, bool measNeigh) const -> void {
        if(measNeigh) {
            BuildSelected(msg, moId, selected);
        } else {
            BuildNonSelected(msg);
        }
    }

private:
    auto NeverConfigured() const -> bool {
        // if never configured, or configured but not confirmed,
        // it means we are not sure if PHY has the config of this
        // MO or not. thus, re-configure is required.
        return !configured || (configured && !confirmed);
        // return !confirmed; // should be enough.
    }

    auto EverConfigured() const -> bool {
        return configured;
    }

private:
    uint8_t configured:1;
    uint8_t confirmed:1;
};

struct LocalMoCfgState {
    MoId    moId;
    CellGroup cellGroup;
    LocalCfgState localFreqState;

    auto BuildInterFreq(PhyMoCfgState& msg, SelectedMo const& selected) const -> void {
        localFreqState.Build(msg.freqChgState, selected, moId, true);
        BuildKey(msg, selected);
    }

protected:
    auto BuildKey(PhyMoCfgState& msg, SelectedMo const& selected) const -> void {
        msg.moId = selected.moId;
        msg.cg = selected.cg; // TODO:
    }
};

struct LocalServingMoCfgState : LocalMoCfgState {
    LocalCfgState localCellState;

    auto BuildSelected(PhyServingMoCfgState& msg, SelectedMo const& selected) const -> void {
        localFreqState.Build(msg.freqChgState, selected, moId, selected.shouldMeasureNeigh);
        localCellState.BuildSelected(msg.cellChgState, moId, selected);
        BuildKey(msg, selected);
    }

    auto BuildNonSelected(PhyServingMoCfgState& msg) const -> void {
        localCellState.BuildNonSelected(msg.cellChgState);
        localFreqState.BuildNonSelected(msg.freqChgState);
    }
};

struct LocalMeasCfgState {
    auto Build(PhyRsMoCfgState& msg, SelectedRsMoList const& selected) -> void {
        pCell.BuildSelected(msg.pCell, selected.pCell);
        BuildPsCell(msg.psCell, selected);
        BuildSCells(msg, selected);
        BuildInterFreqs(msg, selected);
    }

private:
    auto BuildPsCell(PhyServingMoCfgState& msg, SelectedRsMoList const& selected) -> void {
        if(selected.psCell) {
            BuildPsCellWhenSelected(msg, *selected.psCell);
        } else {
            BuildPsCellWhenNotSelected(msg);
        }
    }

    auto BuildPsCellWhenSelected(PhyServingMoCfgState& msg, SelectedMo const& selected) -> void {
        BuildSelectedServingMo(psCell ? &(*psCell) : nullptr, msg, selected);
    }

    auto BuildPsCellWhenNotSelected(PhyServingMoCfgState& msg) -> void {
        if(psCell) {
            psCell->BuildNonSelected(msg);
        } else {
            // Not selected & never configured,
            // Both cell & freq does nothing!!!
            msg.cellChgState.DoNothing();
            msg.freqChgState.DoNothing();
        }
    }

    auto BuildSCells(PhyRsMoCfgState& msg, SelectedRsMoList const& selected) -> void {
        if(selected.sCells.IsEmpty()) {
            msg.shouldReleaseSCells = !sCells.IsEmpty();
        } else {
            selected.sCells.ForEach([this, &phySCells = msg.sCells](SelectedMo const& sCell) {
                BuildSCell(*phySCells.Append(), sCell);
            });
        }
    }

    auto BuildSCell(PhyServingMoCfgState& msg, SelectedMo const& selected) -> void {
        auto* local = sCells.Find([&selected](LocalServingMoCfgState const& local) {
            return selected.moId == local.moId;
        });
        BuildSelectedServingMo(local, msg, selected);
    }

    auto BuildInterFreqs(PhyRsMoCfgState& msg, SelectedRsMoList const& selected) -> void {
        if(selected.interFreq.IsEmpty()) {
            msg.shouldReleaseInterFreq = !interFreqs.IsEmpty();
        } else {
            selected.interFreq.ForEach([this, &phy = msg.interFreq](SelectedMo const& interFreq) {
                BuildInterFreq(*phy.Append(), interFreq);
            });
        }
    }

    auto BuildInterFreq(PhyMoCfgState& msg, SelectedMo const& selected) -> void {
        auto* local = interFreqs.Find([&selected](LocalMoCfgState const& local) {
            return selected.moId == local.moId;
        });
        BuildSelectedInterFreq(local, msg, selected);
    }

    auto BuildSelectedServingMo(LocalServingMoCfgState const* serving, PhyServingMoCfgState& msg, SelectedMo const& selected) -> void {
        if(serving) {
            serving->BuildSelected(msg, selected);
        } else {
            BuildForNewSelected(msg, selected);
        }
    }

    auto BuildSelectedInterFreq(LocalMoCfgState const* interFreq, PhyMoCfgState& msg, SelectedMo const& selected) -> void {
        if(interFreq) {
            interFreq->BuildInterFreq(msg, selected);
        } else {
            msg.freqChgState.ReCfg();
        }
    }

    auto BuildForNewSelected(PhyServingMoCfgState& msg, SelectedMo const& selected) -> void {
        // never configured before
        msg.cellChgState.ReCfg();

        // fully re-config if should measure neighbors,
        // do nothing otherwise.
        selected.shouldMeasureNeigh ?
        msg.freqChgState.ReCfg() : msg.freqChgState.DoNothing();
    }

private:
    LocalServingMoCfgState pCell;
    std::optional<LocalServingMoCfgState> psCell;
    ObjectArray<LocalServingMoCfgState, 3> sCells;
    ObjectArray<LocalMoCfgState, 8> interFreqs;
};


struct LocalPhyMeasCfgState {
    auto Build(PhyMoCfgStateMsg& msg, SelectedMoList const& selected) {
    }

private:
    std::optional<LocalMeasCfgState> ssb;
    std::optional<LocalMeasCfgState> csi;
};
