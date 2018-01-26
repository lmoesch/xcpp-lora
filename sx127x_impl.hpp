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

    Cs::setOutput(xpcc::Gpio::High);

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
    regAccess |= RegAccess::wnr;
    Address_t::set(regAccess, addr);


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
    regAccess |= RegAccess::wnr;
    Address_t::set(regAccess, addr);


    Cs::reset();

    RF_CALL(SpiMaster::transfer(regAccess.value));
    RF_CALL(SpiMaster::transfer(data, nullptr, nbBytes));

	if (this->releaseMaster())
		Cs::set();

    RF_END();
}

// ----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
ResumableResult<void>
SX127x<SpiMaster, Cs>::read(Address addr, uint8_t *data, uint8_t nbBytes)
{
    RF_BEGIN();

    RF_WAIT_UNTIL(this->acquireMaster());

    regAccess &= ~RegAccess::wnr;
    Address_t::set(regAccess, addr);


    Cs::reset();

    RF_CALL(SpiMaster::transfer(regAccess.value));
    RF_CALL(SpiMaster::transfer(nullptr, data, nbBytes));

	if (this->releaseMaster())
		Cs::set();

    RF_END();
}

// ----------------------------------------------------------------------------

}; // end namespace xpcc 
