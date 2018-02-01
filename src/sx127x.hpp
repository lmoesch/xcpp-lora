// ----------------------------------------------------------------------------
/* Copyright (c) 2017, Lucas Mösch
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_SX127x_HPP
#define XPCC_SX127x_HPP

#include "sx127x_header.hpp"

namespace xpcc
{


template <typename SpiMaster, typename Cs>
SX127x<SpiMaster, Cs>::SX127x()
{
    
}

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::initialize()
{
    RF_BEGIN();

    /// Configurate the SPI interface

    RF_END();
}

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::write(Address addr, uint8_t data)
{
    RF_BEGIN();

    RF_WAIT_UNTIL(this->acquireMaster());

    // for write access a '1' is followed by the address
    regAccess.set(RegAccess::wnr);
    Address_t::set(regAccess, addr);

    SpiMaster::setDataMode(SpiMaster::DataMode::Mode0);
    SpiMaster::setDataOrder(SpiMaster::DataOrder::MsbFirst);

    Cs::reset();

    RF_CALL(SpiMaster::transfer(regAccess.value));
    RF_CALL(SpiMaster::transfer(data));

	if (this->releaseMaster())
		Cs::set();

    RF_END();
}

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::write(Address addr, uint8_t *data, uint8_t nbBytes)
{
    RF_BEGIN();

    RF_WAIT_UNTIL(this->acquireMaster());

    // for write access a '1' is followed by the address
    regAccess.set(RegAccess::wnr);
    Address_t::set(regAccess, addr);

    SpiMaster::setDataMode(SpiMaster::DataMode::Mode0);
    SpiMaster::setDataOrder(SpiMaster::DataOrder::MsbFirst);

    Cs::reset();

    RF_CALL(SpiMaster::transfer(regAccess.value));
    RF_CALL(SpiMaster::transfer(data, nullptr, nbBytes));

	if (this->releaseMaster())
		Cs::set();

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::read(Address addr, uint8_t *data, uint8_t nbBytes)
{
    RF_BEGIN();

    RF_WAIT_UNTIL(this->acquireMaster());

    SpiMaster::setDataMode(SpiMaster::DataMode::Mode0);
    SpiMaster::setDataOrder(SpiMaster::DataOrder::MsbFirst);

    regAccess.reset(RegAccess::wnr);
    Address_t::set(regAccess, addr);

    Cs::reset();

    RF_CALL(SpiMaster::transfer(regAccess.value));
    RF_CALL(SpiMaster::transfer(nullptr, data, nbBytes));

	if (this->releaseMaster())
		Cs::set();

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::setLora()
{
    RF_BEGIN();

    /// Put module into sleep mode in order to set LoRa Mode
    RF_CALL(read(Address::OpMode, &((_shared.regOpMode).value), 1));

    Mode_t::set(_shared.regOpMode, Mode::Sleep);

    RF_CALL(write(Address::OpMode, _shared.regOpMode.value));

    /// Set operation mode to LoRa mode
    _shared.regOpMode.set(RegOpMode::LongRangeMode);

    RF_CALL(write(Address::OpMode, _shared.regOpMode.value));

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::setLowFrequencyMode()
{
    RF_BEGIN();

    // Read current configuration and set LowFrequencyMode to 1
    RF_CALL(read(Address::OpMode, &((_shared.regOpMode).value), 1));

    _shared.regOpMode.set(RegOpMode::LowFrequencyModeOn);

    RF_CALL(write(Address::OpMode, _shared.regOpMode.value));

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::setCarrierFreq(uint8_t msb, uint8_t mid, uint8_t lsb)
{
    RF_BEGIN();

    // Read current configuration and set operation mode to 'standby'
    RF_CALL(read(Address::OpMode, &((_shared.regOpMode).value), 1));

    Mode_t::set(_shared.regOpMode, Mode::Standby);

    RF_CALL(write(Address::OpMode, _shared.regOpMode.value));

    // write the three frequency bytes (MSB->LSB)
    RF_CALL(write(Address::FrMsb, msb));
    RF_CALL(write(Address::FrMid, mid));
    RF_CALL(write(Address::FrLsb, lsb));

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::setPaBoost()
{
    RF_BEGIN();

    // Read current configuration and set operation mode to 'standby'
    RF_CALL(read(Address::PaConfig, &((_shared.regPaConfig).value), 1));

    _shared.regPaConfig.set(RegPaConfig::PaSelect);

    RF_CALL(write(Address::PaConfig, _shared.regPaConfig.value));

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::setOutputPower(uint8_t power)
{
    RF_BEGIN();

    // Read current configuration and set operation mode to 'standby'
    RF_CALL(read(Address::PaConfig, &((_shared.regPaConfig).value), 1));

    OutputPower_t::set(_shared.regPaConfig, power);

    RF_CALL(write(Address::PaConfig, _shared.regPaConfig.value));

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::setBandwidth(SignalBandwidth bw)
{
    RF_BEGIN();

    RF_CALL(read(Address::ModemConfig1, &((_shared.regModemConfig1).value), 1));

    SignalBandwidth_t::set(_shared.regModemConfig1, bw);

    RF_CALL(write(Address::ModemConfig1, _shared.regModemConfig1.value));

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::setCodingRate(ErrorCodingRate cr)
{
    RF_BEGIN();

    RF_CALL(read(Address::ModemConfig1, &((_shared.regModemConfig1).value), 1));

    ErrorCodingRate_t::set(_shared.regModemConfig1, cr);

    RF_CALL(write(Address::ModemConfig1, _shared.regModemConfig1.value));

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::setImplicitHeaderMode()
{
    RF_BEGIN();

    RF_CALL(read(Address::ModemConfig1, &((_shared.regModemConfig1).value), 1));

    _shared.regModemConfig1.set(RegModemConfig1::ImplicitHeaderModeOn);

    RF_CALL(write(Address::ModemConfig1, _shared.regModemConfig1.value));

    RF_END();
};

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::setExplicitHeaderMode()
{
    RF_BEGIN();

    RF_CALL(read(Address::ModemConfig1, &((_shared.regModemConfig1).value), 1));

    _shared.regModemConfig1.reset(RegModemConfig1::ImplicitHeaderModeOn);

    RF_CALL(write(Address::ModemConfig1, _shared.regModemConfig1.value));

    RF_END();
};

} // end namespace xpcc 

#endif // XPCC_SX127x_HPP