package dc_simulations.modules.BouncingSwitchRelay;

//
// Extended relay module that includes Decision Tree based deflection
//
simple BouncingIeee8021dRelayWithDT extends BouncingIeee8021dRelay
{
    parameters:
        @class(BouncingIeee8021dRelayWithDT);
        @display("i=block/broadcast;bgb=425,114");
}