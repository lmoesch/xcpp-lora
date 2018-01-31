// ----------------------------------------------------------------------------
/* Copyright (c) 2017, Lucas Mösch
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_SX127x_HPP
#	error "Don't include this file directly! Use 'sx127x.hpp' instead."
#endif

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

} // end namespace xpcc 
