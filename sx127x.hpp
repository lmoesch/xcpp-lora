// ----------------------------------------------------------------------------
/* Copyright (c) 2018, Lucas Moesch
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_SX127x_HPP
#define XPCC_SX127x_HPP

#include <xpcc/architecture/interface/spi_device.hpp>
#include <xpcc/architecture/driver/accessor.hpp>
#include <xpcc/processing.hpp>

namespace xpcc
{

struct sx127x
{
public:
    enum class
    RegAddr : uint8_t
    {
        Fifo = 0x00,

        // -- Common Registers -------------------------------------------------
        
        OpMode = 0x01,
        FrMsb = 0x06,
        FrMid = 0x07,
        FrLsb = 0x08,

        // -- RF Block Registers -----------------------------------------------

        PaConfig = 0x09,

    };


    // -- Common Registers -----------------------------------------------------

    // -- Operation Mode 
    enum class 
    RegOpMode : uint8_t
    {
        /// Sets device in either FSK/OOK mode (0) or LoRa mode (1) 
        LongRangeMode = Bit7, 

        // Bit6: AccessSharedReg
        // Bit5: Reserved
        // But4: Reserved

        /// Enables the high frequency driver (0) or the low frequency one (1)
        LowFrequencyModeOn = Bit3, 
    };
    XPCC_FLAGS8(RegOpMode)

    enum class 
    Mode : uint8_t
    {
        Sleep = 0,
        Standby = Bit0,
        FreqSynthTX = Bit1,
        Transmit = Bit1 | Bit0,
        FreqSynthRX = Bit2,
        RecvCont = Bit2 | Bit0,
        RecvSingle = Bit2 | Bit1,
        ChnActvDetect = Bit2 | Bit1 | Bit0 
    };
    typedef xpcc::Configuration<RegOpMode_t, Mode, 0b111> Mode_t;

    // --

    // -- RF Block Registers ---------------------------------------------------

    // -- PA Config 
    enum class
    RegPaConfig : uint8_t
    {
        /// Selects PA output pin, either RFO (0) or PA_BOOST (1)
        PaSelect = Bit7
    };
    XPCC_FLAGS8(RegPaConfig)

    typedef xpcc::Value<RegPaConfig_t, 3, 0x04>  MaxPower_t;
    typedef xpcc::Value<RegPaConfig_t, 4, 0x00>  OutputPower_t;

    // -- PA Ramp
    enum class
    RegPaRamp : uint8_t
    {};
    XPCC_FLAGS8(RegPaRamp)

    typedef xpcc::Value<RegPaRamp_t, 4, 0x00> PaRamp_t;

    // --

};

struct SX127x_conf 
{
    uint8_t maxBuffer;
};

/**
 * Driver for SX1276/7/8 based FSK/OOK, LoRa modules.
 *
 * Nice description.
 *
 * @tparam	SpiMaster	SpiMaster interface
 * @tparam	Cs			Chip-select pin
 *
 * @author	Lucas Moesch
 * @ingroup ???
 */
template <typename SpiMaster, typename Cs>
class SX127x : public sx127x, public xpcc::SpiDevice<SpiMaster>, protected xpcc::NestedResumable<3>
{
public:
	SX127x();

	/// 
	xpcc::ResumableResult<void>
	initialize();

    xpcc::ResumableResult<void>
    write(uint8_t addr, uint8_t *data, uint8_t nbBytes);

    xpcc::ResumableResult<void>
    read(uint8_t addr, uint8_t *data, uint8_t nbBytes);

private:
    uint8_t firstByte;

    RegOpMode_t operationMode;
};
} // Namespace xpcc

#include "sx127x_impl.hpp"

#endif // XPCC_SX127x_HPP