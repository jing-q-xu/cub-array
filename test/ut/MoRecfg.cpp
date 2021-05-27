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

    uint8_t present : 1;
    uint8_t changed : 1;

    auto DoNothing() -> void {
        present = false;
        changed = false;
    }

    auto ReCfg() -> void {
        present = true;
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
};

struct SelectedMoCfgState {
    MoId    moId;
    CellGroup cg;

    auto IsCfgChanged(MoId moId) const -> bool {
        return (changed || moId != this->moId);
    }

    auto BuildNeigh(PhyCfgChangeState& phy, MoId moId) const -> void {
        phy.present = shouldMeasureNeigh;
        phy.changed = phy.present ? IsCfgChanged(moId) : false;
    }

    uint8_t changed:1;
    uint8_t shouldMeasureNeigh:1;
};

struct SelectedRsMoCfgState {
    SelectedMoCfgState pCell;
    std::optional<SelectedMoCfgState> psCell;

    ObjectArray<SelectedMoCfgState, 3> sCells;
    ObjectArray<SelectedMoCfgState, 8> interFreq;
};



struct LocalCfgState {
    LocalCfgState() : configured{false}, confirmed{false} {}

    auto Build(PhyCfgChangeState& phy) const -> void {
        if(phy.present) {
            if(ShouldFullReCfg()) {
                phy.changed = true;
            }
        } else {
            // Release!!! if ever configured, even never confirmed.
            phy.changed = configured;
        }
    }

private:
    auto ShouldFullReCfg() const -> bool {
        // if never configured, or configured but not confirmed,
        // full-configure again.
        return !configured || (configured && !confirmed);
    }

private:

    uint8_t configured:1;
    uint8_t confirmed:1;
};

struct LocalMoCfgState {
    MoId    moId;
    CellGroup cellGroup;
    LocalCfgState freqState;

    auto BuildFreq(PhyMoCfgState& msg, SelectedMoCfgState const& selected) const -> void {
        msg.moId = selected.moId;
        msg.cg = selected.cg; // TODO:

        selected.BuildNeigh(msg.freqChgState, moId);
        freqState.Build(msg.freqChgState);
    }
};

struct LocalServingMoCfgState : LocalMoCfgState {
    LocalCfgState cellState;

    auto BuildSelected(PhyServingMoCfgState& msg, SelectedMoCfgState const& selected) const -> void {
        BuildFreq(msg, selected);
        BuildCell(msg.cellChgState, selected);
    }

    auto BuildNonSelected(PhyServingMoCfgState& msg) const -> void {
        BuildNonSelectedFreq(msg.freqChgState);
        BuildNonSelectedCell(msg.cellChgState);
    }

private:
    auto BuildCell(PhyCfgChangeState& cell, SelectedMoCfgState const& selected) const -> void {
        cell.present = true;
        cell.changed = selected.IsCfgChanged(moId);
        cellState.Build(cell);
    }

    auto BuildNonSelectedFreq(PhyCfgChangeState& freq) const -> void {
        freq.present = false;
        freqState.Build(freq);
    }

    auto BuildNonSelectedCell(PhyCfgChangeState& cell) const -> void {
        cell.present = false;
        cellState.Build(cell);
    }
};

struct LocalMeasCfgState {
    auto Build(PhyRsMoCfgState& msg, SelectedRsMoCfgState const& selected) -> void {
        pCell.BuildSelected(msg.pCell, selected.pCell);
        BuildPsCell(msg.psCell, selected);
        BuildSCells(msg, selected);
        BuildInterFreqs(msg, selected);
    }

private:
    auto BuildPsCell(PhyServingMoCfgState& msg, SelectedRsMoCfgState const& selected) -> void {
        if(selected.psCell) {
            BuildPsCellWhenSelected(msg, *selected.psCell);
        } else {
            BuildPsCellWhenNotSelected(msg);
        }
    }

    auto BuildPsCellWhenSelected(PhyServingMoCfgState& msg, SelectedMoCfgState const& selected) -> void {
        BuildSelectedServingMo(psCell ? &(*psCell) : nullptr, msg, selected);
    }

    auto BuildPsCellWhenNotSelected(PhyServingMoCfgState& msg) -> void {
        if(psCell) {
            psCell->BuildNonSelected(msg);
        } else {
            // Both cell & freq does nothing!!!
            msg.cellChgState.DoNothing();
            msg.freqChgState.DoNothing();
        }
    }

    auto BuildSCells(PhyRsMoCfgState& msg, SelectedRsMoCfgState const& selected) -> void {
        selected.sCells.ForEach([this, &phySCells = msg.sCells](SelectedMoCfgState const& sCell) {
            BuildSCell(*phySCells.Append(), sCell);
        });
    }

    auto BuildSCell(PhyServingMoCfgState& msg, SelectedMoCfgState const& selected) -> void {
        auto* local = sCells.Find([&selected](LocalServingMoCfgState const& local) {
            return selected.moId == local.moId;
        });
        BuildSelectedServingMo(local, msg, selected);
    }

    auto BuildInterFreqs(PhyRsMoCfgState& msg, SelectedRsMoCfgState const& selected) -> void {
        selected.interFreq.ForEach([this, &phy = msg.interFreq](SelectedMoCfgState const& interFreq) {
            BuildInterFreq(*phy.Append(), interFreq);
        });
    }

    auto BuildInterFreq(PhyMoCfgState& msg, SelectedMoCfgState const& selected) -> void {
        auto* local = interFreqs.Find([&selected](LocalMoCfgState const& local) {
            return selected.moId == local.moId;
        });
        BuildSelectedInterFreq(local, msg, selected);
    }

    auto BuildSelectedServingMo(LocalServingMoCfgState const* serving, PhyServingMoCfgState& msg, SelectedMoCfgState const& selected) -> void {
        if(serving) {
            serving->BuildSelected(msg, selected);
        } else {
            BuildForNewSelected(msg, selected);
        }
    }

    auto BuildSelectedInterFreq(LocalMoCfgState const* interFreq, PhyMoCfgState& msg, SelectedMoCfgState const& selected) -> void {
        if(interFreq) {
            interFreq->BuildFreq(msg, selected);
        } else {
            msg.freqChgState.ReCfg();
        }
    }

    auto BuildForNewSelected(PhyServingMoCfgState& msg, SelectedMoCfgState const& selected) -> void {
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


struct PhyMeasCfgState : ObjectArray<PhyRsMoCfgState, 2> {};
