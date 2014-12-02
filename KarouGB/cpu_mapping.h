//
//  cpu_mapping.h
//  mygb
//
//  Created by Daniel on 28.06.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef mygb_cpu_mapping_h
#define mygb_cpu_mapping_h

OpFptr Z80::optable_00[256] = {
    //0
    &opfuncs::NOP,      &opfuncs::LDBCnn,   &opfuncs::LDBCmA,   &opfuncs::INCBC,    &opfuncs::INCr_b,   &opfuncs::DECr_b,
    &opfuncs::LDrn_b,   &opfuncs::RLCA,     &opfuncs::LDmmSP,   &opfuncs::ADDHLBC,  &opfuncs::LDABCm,   &opfuncs::DECBC,
    &opfuncs::INCr_c,   &opfuncs::DECr_c,   &opfuncs::LDrn_c,   &opfuncs::RRCA,
    
    //10
    &opfuncs::STOP,     &opfuncs::LDDEnn,   &opfuncs::LDDEmA,   &opfuncs::INCDE,    &opfuncs::INCr_d,   &opfuncs::DECr_d,
    &opfuncs::LDrn_d,   &opfuncs::RLA,      &opfuncs::JRn,      &opfuncs::ADDHLDE,  &opfuncs::LDADEm,   &opfuncs::DECDE,
    &opfuncs::INCr_e,   &opfuncs::DECr_e,   &opfuncs::LDrn_e,   &opfuncs::RRA,
    
    //20
    &opfuncs::JRNZn,    &opfuncs::LDHLnn,   &opfuncs::LDHLIA,   &opfuncs::INCHL,    &opfuncs::INCr_h,   &opfuncs::DECr_h,
    &opfuncs::LDrn_h,   &opfuncs::DAA,      &opfuncs::JRZn,     &opfuncs::ADDHLHL,  &opfuncs::LDAHLI,   &opfuncs::DECHL,
    &opfuncs::INCr_l,   &opfuncs::DECr_l,   &opfuncs::LDrn_l,   &opfuncs::CPL,
    
    //30
    &opfuncs::JRNCn,    &opfuncs::LDSPnn,   &opfuncs::LDHLDA,   &opfuncs::INCSP,    &opfuncs::INCHLm,   &opfuncs::DECHLm,
    &opfuncs::LDHLmn,   &opfuncs::SCF,      &opfuncs::JRCn,     &opfuncs::ADDHLSP,  &opfuncs::LDAHLD,   &opfuncs::DECSP,
    &opfuncs::INCr_a,   &opfuncs::DECr_a,   &opfuncs::LDrn_a,   &opfuncs::CCF,
    
    //40
    &opfuncs::LDrr_bb,  &opfuncs::LDrr_bc, &opfuncs::LDrr_bd,   &opfuncs::LDrr_be,  &opfuncs::LDrr_bh,  &opfuncs::LDrr_bl,
    &opfuncs::LDrHLm_b, &opfuncs::LDrr_ba,  &opfuncs::LDrr_cb,  &opfuncs::LDrr_cc,  &opfuncs::LDrr_cd,  &opfuncs::LDrr_ce,
    &opfuncs::LDrr_ch,  &opfuncs::LDrr_cl,  &opfuncs::LDrHLm_c, &opfuncs::LDrr_ca,
    
    //50
    &opfuncs::LDrr_db,  &opfuncs::LDrr_dc,  &opfuncs::LDrr_dd,  &opfuncs::LDrr_de,  &opfuncs::LDrr_dh,  &opfuncs::LDrr_dl,
    &opfuncs::LDrHLm_d, &opfuncs::LDrr_da,  &opfuncs::LDrr_eb,  &opfuncs::LDrr_ec,  &opfuncs::LDrr_ed,  &opfuncs::LDrr_ee,
    &opfuncs::LDrr_eh,  &opfuncs::LDrr_el,  &opfuncs::LDrHLm_e, &opfuncs::LDrr_ea,
    
    
    &opfuncs::LDrr_hb,  &opfuncs::LDrr_hc,  &opfuncs::LDrr_hd,  &opfuncs::LDrr_he,  &opfuncs::LDrr_hh,  &opfuncs::LDrr_hl,
    &opfuncs::LDrHLm_h, &opfuncs::LDrr_ha,  &opfuncs::LDrr_lb,  &opfuncs::LDrr_lc,  &opfuncs::LDrr_ld,  &opfuncs::LDrr_le,
    &opfuncs::LDrr_lh,  &opfuncs::LDrr_ll,  &opfuncs::LDrHLm_l, &opfuncs::LDrr_la,
    
    //70
    &opfuncs::LDHLmr_b, &opfuncs::LDHLmr_c, &opfuncs::LDHLmr_d, &opfuncs::LDHLmr_e, &opfuncs::LDHLmr_h, &opfuncs::LDHLmr_l,
    &opfuncs::HALT,     &opfuncs::LDHLmr_a, &opfuncs::LDrr_ab,  &opfuncs::LDrr_ac,  &opfuncs::LDrr_ad,  &opfuncs::LDrr_ae,
    &opfuncs::LDrr_ah,  &opfuncs::LDrr_al,  &opfuncs::LDrHLm_a, &opfuncs::LDrr_aa,
    
    // 80
    &opfuncs::ADDr_b,   &opfuncs::ADDr_c,   &opfuncs::ADDr_d,   &opfuncs::ADDr_e,   &opfuncs::ADDr_h,   &opfuncs::ADDr_l,
    &opfuncs::ADDHL,    &opfuncs::ADDr_a,   &opfuncs::ADCr_b,   &opfuncs::ADCr_c,   &opfuncs::ADCr_d,   &opfuncs::ADCr_e,
    &opfuncs::ADCr_h,   &opfuncs::ADCr_l,   &opfuncs::ADCHL,    &opfuncs::ADCr_a,
    
    // 90
    &opfuncs::SUBr_b,   &opfuncs::SUBr_c,   &opfuncs::SUBr_d,   &opfuncs::SUBr_e,   &opfuncs::SUBr_h,   &opfuncs::SUBr_l,
    &opfuncs::SUBHL,    &opfuncs::SUBr_a,   &opfuncs::SBCr_b,   &opfuncs::SBCr_c,   &opfuncs::SBCr_d,   &opfuncs::SBCr_e,
    &opfuncs::SBCr_h,   &opfuncs::SBCr_l,   &opfuncs::SBCHL,    &opfuncs::SBCr_a,
    
    // A0
    &opfuncs::ANDr_b,   &opfuncs::ANDr_c,   &opfuncs::ANDr_d,   &opfuncs::ANDr_e,   &opfuncs::ANDr_h,   &opfuncs::ANDr_l,
    &opfuncs::ANDHL,    &opfuncs::ANDr_a,   &opfuncs::XORr_b,   &opfuncs::XORr_c,   &opfuncs::XORr_d,   &opfuncs::XORr_e,
    &opfuncs::XORr_h,   &opfuncs::XORr_l,   &opfuncs::XORHL,    &opfuncs::XORr_a,
    
    // B0
    &opfuncs::ORr_b,    &opfuncs::ORr_c,    &opfuncs::ORr_d,    &opfuncs::ORr_e,    &opfuncs::ORr_h,    &opfuncs::ORr_l,
    &opfuncs::ORHL,     &opfuncs::ORr_a,    &opfuncs::CPr_b,    &opfuncs::CPr_c,    &opfuncs::CPr_d,    &opfuncs::CPr_e,
    &opfuncs::CPr_h,    &opfuncs::CPr_l,    &opfuncs::CPHL,     &opfuncs::CPr_a,
    
    // C0
    &opfuncs::RETNZ,    &opfuncs::POPBC,    &opfuncs::JPNZnn,   &opfuncs::JPnn,     &opfuncs::CALLNZnn, &opfuncs::PUSHBC,
    &opfuncs::ADDn,     &opfuncs::RST00,    &opfuncs::RETZ,     &opfuncs::RET,      &opfuncs::JPZnn,    nullptr,/*CB-Prefix*/
    &opfuncs::CALLZnn,  &opfuncs::CALLnn,   &opfuncs::ADCn,     &opfuncs::RST08,
    
    // D0
    &opfuncs::RETNC,    &opfuncs::POPDE,    &opfuncs::JPNCnn,   nullptr,            &opfuncs::CALLNCnn, &opfuncs::PUSHDE,
    &opfuncs::SUBn,     &opfuncs::RST10,    &opfuncs::RETC,     &opfuncs::RETI,     &opfuncs::JPCnn,    nullptr,
    &opfuncs::CALLCnn,  nullptr,            &opfuncs::SBCn,     &opfuncs::RST18,
    
    // E0
    &opfuncs::LDIOnA,   &opfuncs::POPHL,    &opfuncs::LDIOCA,   nullptr,            nullptr,            &opfuncs::PUSHHL,
    &opfuncs::ANDn,     &opfuncs::RST20,    &opfuncs::ADDSPn,   &opfuncs::JPHL,     &opfuncs::LDmmA,    nullptr,
    nullptr,            nullptr,            &opfuncs::XORn,     &opfuncs::RST28,
    
    // F0
    &opfuncs::LDAIOn,   &opfuncs::POPAF,    &opfuncs::LDAIOC,   &opfuncs::DI,       nullptr,            &opfuncs::PUSHAF,
    &opfuncs::ORn,      &opfuncs::RST30,    &opfuncs::LDHLSPn,  &opfuncs::LDSPHL,   &opfuncs::LDAmm,    &opfuncs::EI,
    nullptr,            nullptr,            &opfuncs::CPn,      &opfuncs::RST38,
};


OpFptr Z80::optable_CB[256] = {
    
    //CB00
    &opfuncs::RLCr_b,   &opfuncs::RLCr_c,   &opfuncs::RLCr_d,   &opfuncs::RLCr_e,   &opfuncs::RLCr_h,   &opfuncs::RLCr_l,
    &opfuncs::RLCHL,    &opfuncs::RLCr_a,   &opfuncs::RRCr_b,   &opfuncs::RRCr_c,   &opfuncs::RRCr_d,   &opfuncs::RRCr_e,
    &opfuncs::RRCr_h,   &opfuncs::RRCr_l,   &opfuncs::RRCHL,    &opfuncs::RRCr_a,
    
    // CB10
    &opfuncs::RLr_b,    &opfuncs::RLr_c,    &opfuncs::RLr_d,    &opfuncs::RLr_e,    &opfuncs::RLr_h,    &opfuncs::RLr_l,
    &opfuncs::RLHL,     &opfuncs::RLr_a,    &opfuncs::RRr_b,    &opfuncs::RRr_c,    &opfuncs::RRr_d,    &opfuncs::RRr_e,
    &opfuncs::RRr_h,    &opfuncs::RRr_l,    &opfuncs::RRHL,     &opfuncs::RRr_a,
    
    // CB20
    &opfuncs::SLAr_b,   &opfuncs::SLAr_c,   &opfuncs::SLAr_d,   &opfuncs::SLAr_e,   &opfuncs::SLAr_h,   &opfuncs::SLAr_l,
    &opfuncs::SLAHL,    &opfuncs::SLAr_a,   &opfuncs::SRAr_b,   &opfuncs::SRAr_c,   &opfuncs::SRAr_d,   &opfuncs::SRAr_e,
    &opfuncs::SRAr_h,   &opfuncs::SRAr_l,   &opfuncs::SRAHL,    &opfuncs::SRAr_a,
    
    // CB30
    &opfuncs::SWAPr_b,  &opfuncs::SWAPr_c,  &opfuncs::SWAPr_d,  &opfuncs::SWAPr_e,  &opfuncs::SWAPr_h,  &opfuncs::SWAPr_l,
    &opfuncs::SWAPHLm,  &opfuncs::SWAPr_a,  &opfuncs::SRLr_b,   &opfuncs::SRLr_c,   &opfuncs::SRLr_d,   &opfuncs::SRLr_e,
    &opfuncs::SRLr_h,   &opfuncs::SRLr_l,   &opfuncs::SRLHL,    &opfuncs::SRLr_a,
    
    // CB40
    &opfuncs::BIT0b,    &opfuncs::BIT0c,    &opfuncs::BIT0d,    &opfuncs::BIT0e,    &opfuncs::BIT0h,    &opfuncs::BIT0l,
    &opfuncs::BIT0n,    &opfuncs::BIT0a,    &opfuncs::BIT1b,    &opfuncs::BIT1c,    &opfuncs::BIT1d,    &opfuncs::BIT1e,
    &opfuncs::BIT1h,    &opfuncs::BIT1l,    &opfuncs::BIT1n,    &opfuncs::BIT1a,
    
    // CB50
    &opfuncs::BIT2b,    &opfuncs::BIT2c,    &opfuncs::BIT2d,    &opfuncs::BIT2e,    &opfuncs::BIT2h,    &opfuncs::BIT2l,
    &opfuncs::BIT2n,    &opfuncs::BIT2a,    &opfuncs::BIT3b,    &opfuncs::BIT3c,    &opfuncs::BIT3d,    &opfuncs::BIT3e,
    &opfuncs::BIT3h,    &opfuncs::BIT3l,    &opfuncs::BIT3n,    &opfuncs::BIT3a,
    
    // CB60
    &opfuncs::BIT4b,    &opfuncs::BIT4c,    &opfuncs::BIT4d,    &opfuncs::BIT4e,    &opfuncs::BIT4h,    &opfuncs::BIT4l,
    &opfuncs::BIT4n,    &opfuncs::BIT4a,    &opfuncs::BIT5b,    &opfuncs::BIT5c,    &opfuncs::BIT5d,    &opfuncs::BIT5e,
    &opfuncs::BIT5h,    &opfuncs::BIT5l,    &opfuncs::BIT5n,    &opfuncs::BIT5a,
    
    // CB70
    &opfuncs::BIT6b,    &opfuncs::BIT6c,    &opfuncs::BIT6d,    &opfuncs::BIT6e,    &opfuncs::BIT6h,    &opfuncs::BIT6l,
    &opfuncs::BIT6n,    &opfuncs::BIT6a,    &opfuncs::BIT7b,    &opfuncs::BIT7c,    &opfuncs::BIT7d,    &opfuncs::BIT7e,
    &opfuncs::BIT7h,    &opfuncs::BIT7l,    &opfuncs::BIT7n,    &opfuncs::BIT7a,
    
    // CB80
    &opfuncs::RES0b,    &opfuncs::RES0c,    &opfuncs::RES0d,    &opfuncs::RES0e,    &opfuncs::RES0h,    &opfuncs::RES0l,
    &opfuncs::RES0m,    &opfuncs::RES0a,    &opfuncs::RES1b,    &opfuncs::RES1c,    &opfuncs::RES1d,    &opfuncs::RES1e,
    &opfuncs::RES1h,    &opfuncs::RES1l,    &opfuncs::RES1m,    &opfuncs::RES1a,
    
    // CB90
    &opfuncs::RES2b,    &opfuncs::RES2c,    &opfuncs::RES2d,    &opfuncs::RES2e,    &opfuncs::RES2h,    &opfuncs::RES2l,
    &opfuncs::RES2m,    &opfuncs::RES2a,    &opfuncs::RES3b,    &opfuncs::RES3c,    &opfuncs::RES3d,    &opfuncs::RES3e,
    &opfuncs::RES3h,    &opfuncs::RES3l,    &opfuncs::RES3m,    &opfuncs::RES3a,
    
    // CBA0
    &opfuncs::RES4b,    &opfuncs::RES4c,    &opfuncs::RES4d,    &opfuncs::RES4e,    &opfuncs::RES4h,    &opfuncs::RES4l,
    &opfuncs::RES4m,    &opfuncs::RES4a,    &opfuncs::RES5b,    &opfuncs::RES5c,    &opfuncs::RES5d,    &opfuncs::RES5e,
    &opfuncs::RES5h,    &opfuncs::RES5l,    &opfuncs::RES5m,    &opfuncs::RES5a,
    
    // CBB0
    &opfuncs::RES6b,    &opfuncs::RES6c,    &opfuncs::RES6d,    &opfuncs::RES6e,    &opfuncs::RES6h,    &opfuncs::RES6l,
    &opfuncs::RES6m,    &opfuncs::RES6a,    &opfuncs::RES7b,    &opfuncs::RES7c,    &opfuncs::RES7d,    &opfuncs::RES7e,
    &opfuncs::RES7h,    &opfuncs::RES7l,    &opfuncs::RES7m,    &opfuncs::RES7a,
    
    // CBC0
    &opfuncs::SET0b,    &opfuncs::SET0c,    &opfuncs::SET0d,    &opfuncs::SET0e,    &opfuncs::SET0h,    &opfuncs::SET0l,
    &opfuncs::SET0m,    &opfuncs::SET0a,    &opfuncs::SET1b,    &opfuncs::SET1c,    &opfuncs::SET1d,    &opfuncs::SET1e,
    &opfuncs::SET1h,    &opfuncs::SET1l,    &opfuncs::SET1m,    &opfuncs::SET1a,
    
    // CBD0
    &opfuncs::SET2b,    &opfuncs::SET2c,    &opfuncs::SET2d,    &opfuncs::SET2e,    &opfuncs::SET2h,    &opfuncs::SET2l,
    &opfuncs::SET2m,    &opfuncs::SET2a,    &opfuncs::SET3b,    &opfuncs::SET3c,    &opfuncs::SET3d,    &opfuncs::SET3e,
    &opfuncs::SET3h,    &opfuncs::SET3l,    &opfuncs::SET3m,    &opfuncs::SET3a,
    
    // CBE0
    &opfuncs::SET4b,    &opfuncs::SET4c,    &opfuncs::SET4d,    &opfuncs::SET4e,    &opfuncs::SET4h,    &opfuncs::SET4l,
    &opfuncs::SET4m,    &opfuncs::SET4a,    &opfuncs::SET5b,    &opfuncs::SET5c,    &opfuncs::SET5d,    &opfuncs::SET5e,
    &opfuncs::SET5h,    &opfuncs::SET5l,    &opfuncs::SET5m,    &opfuncs::SET5a,
    
    // CBF0
    &opfuncs::SET6b,    &opfuncs::SET6c,    &opfuncs::SET6d,    &opfuncs::SET6e,    &opfuncs::SET6h,    &opfuncs::SET6l,
    &opfuncs::SET6m,    &opfuncs::SET6a,    &opfuncs::SET7b,    &opfuncs::SET7c,    &opfuncs::SET7d,    &opfuncs::SET7e,
    &opfuncs::SET7h,    &opfuncs::SET7l,    &opfuncs::SET7m,    &opfuncs::SET7a

};


#endif
