namespace xpcc
{


template <typename SpiMaster, typename Cs>
SX127x<SpiMaster, Cs, SX127x_conf>::SX127x()
{
    
}

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs, SX127x_conf>::initialize()
{
    RF_BEGIN();

    Cs::setOutput(xpcc::Gpio::High);

    RF_END();
}

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs, SX127x_conf>::write(RegAddr addr, uint8_t *data, uint8_t nbBytes)
{
    RF_BEGIN();

    RF_WAIT_UNTIL(this->acquireMaster());

    // for write access a '1' is followed by the address
    firstByte = 0x80 | addr;

    Cs::reset();

    RF_CALL(SpiMaster::transfer(buffer, nullptr, 1));
    RF_CALL(SpiMaster::transfer(data, nullptr, nbBytes));

	if (this->releaseMaster())
		Cs::set();

    RF_END();
}

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs, SX127x_conf>::read(RegAddr addr, uint8_t *data, uint8_t nbBytes)
{
    RF_BEGIN();

    RF_WAIT_UNTIL(this->acquireMaster());

    firstByte = 0x00 | addr;

    Cs::reset();

    RF_CALL(SpiMaster::transfer(firstByte, nullptr, 1));
    RF_CALL(SpiMaster::transfer(nullptr, data, nbBytes));

	if (this->releaseMaster())
		Cs::set();

    RF_END();
}

// ----------------------------------------------------------------------------

}; // end namespace xpcc 
