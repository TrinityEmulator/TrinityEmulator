/*
 * Ported from a work by Andreas Grabher for Previous, NeXT Computer Emulator,
 * derived from NetBSD M68040 FPSP functions,
 * derived from release 2a of the SoftFloat IEC/IEEE Floating-point Arithmetic
 * Package. Those parts of the code (and some later contributions) are
 * provided under that license, as detailed below.
 * It has subsequently been modified by contributors to the QEMU Project,
 * so some portions are provided under:
 *  the SoftFloat-2a license
 *  the BSD license
 *  GPL-v2-or-later
 *
 * Any future contributions to this file will be taken to be licensed under
 * the Softfloat-2a license unless specifically indicated otherwise.
 */

/*
 * Portions of this work are licensed under the terms of the GNU GPL,
 * version 2 or later. See the COPYING file in the top-level directory.
 */

#ifndef TARGET_M68K_SOFTFLOAT_FPSP_TABLES_H
#define TARGET_M68K_SOFTFLOAT_FPSP_TABLES_H

static const floatx80 log_tbl[128] = {
    make_floatx80_init(0x3FFE, 0xFE03F80FE03F80FE),
    make_floatx80_init(0x3FF7, 0xFF015358833C47E2),
    make_floatx80_init(0x3FFE, 0xFA232CF252138AC0),
    make_floatx80_init(0x3FF9, 0xBDC8D83EAD88D549),
    make_floatx80_init(0x3FFE, 0xF6603D980F6603DA),
    make_floatx80_init(0x3FFA, 0x9CF43DCFF5EAFD48),
    make_floatx80_init(0x3FFE, 0xF2B9D6480F2B9D65),
    make_floatx80_init(0x3FFA, 0xDA16EB88CB8DF614),
    make_floatx80_init(0x3FFE, 0xEF2EB71FC4345238),
    make_floatx80_init(0x3FFB, 0x8B29B7751BD70743),
    make_floatx80_init(0x3FFE, 0xEBBDB2A5C1619C8C),
    make_floatx80_init(0x3FFB, 0xA8D839F830C1FB49),
    make_floatx80_init(0x3FFE, 0xE865AC7B7603A197),
    make_floatx80_init(0x3FFB, 0xC61A2EB18CD907AD),
    make_floatx80_init(0x3FFE, 0xE525982AF70C880E),
    make_floatx80_init(0x3FFB, 0xE2F2A47ADE3A18AF),
    make_floatx80_init(0x3FFE, 0xE1FC780E1FC780E2),
    make_floatx80_init(0x3FFB, 0xFF64898EDF55D551),
    make_floatx80_init(0x3FFE, 0xDEE95C4CA037BA57),
    make_floatx80_init(0x3FFC, 0x8DB956A97B3D0148),
    make_floatx80_init(0x3FFE, 0xDBEB61EED19C5958),
    make_floatx80_init(0x3FFC, 0x9B8FE100F47BA1DE),
    make_floatx80_init(0x3FFE, 0xD901B2036406C80E),
    make_floatx80_init(0x3FFC, 0xA9372F1D0DA1BD17),
    make_floatx80_init(0x3FFE, 0xD62B80D62B80D62C),
    make_floatx80_init(0x3FFC, 0xB6B07F38CE90E46B),
    make_floatx80_init(0x3FFE, 0xD3680D3680D3680D),
    make_floatx80_init(0x3FFC, 0xC3FD032906488481),
    make_floatx80_init(0x3FFE, 0xD0B69FCBD2580D0B),
    make_floatx80_init(0x3FFC, 0xD11DE0FF15AB18CA),
    make_floatx80_init(0x3FFE, 0xCE168A7725080CE1),
    make_floatx80_init(0x3FFC, 0xDE1433A16C66B150),
    make_floatx80_init(0x3FFE, 0xCB8727C065C393E0),
    make_floatx80_init(0x3FFC, 0xEAE10B5A7DDC8ADD),
    make_floatx80_init(0x3FFE, 0xC907DA4E871146AD),
    make_floatx80_init(0x3FFC, 0xF7856E5EE2C9B291),
    make_floatx80_init(0x3FFE, 0xC6980C6980C6980C),
    make_floatx80_init(0x3FFD, 0x82012CA5A68206D7),
    make_floatx80_init(0x3FFE, 0xC4372F855D824CA6),
    make_floatx80_init(0x3FFD, 0x882C5FCD7256A8C5),
    make_floatx80_init(0x3FFE, 0xC1E4BBD595F6E947),
    make_floatx80_init(0x3FFD, 0x8E44C60B4CCFD7DE),
    make_floatx80_init(0x3FFE, 0xBFA02FE80BFA02FF),
    make_floatx80_init(0x3FFD, 0x944AD09EF4351AF6),
    make_floatx80_init(0x3FFE, 0xBD69104707661AA3),
    make_floatx80_init(0x3FFD, 0x9A3EECD4C3EAA6B2),
    make_floatx80_init(0x3FFE, 0xBB3EE721A54D880C),
    make_floatx80_init(0x3FFD, 0xA0218434353F1DE8),
    make_floatx80_init(0x3FFE, 0xB92143FA36F5E02E),
    make_floatx80_init(0x3FFD, 0xA5F2FCABBBC506DA),
    make_floatx80_init(0x3FFE, 0xB70FBB5A19BE3659),
    make_floatx80_init(0x3FFD, 0xABB3B8BA2AD362A5),
    make_floatx80_init(0x3FFE, 0xB509E68A9B94821F),
    make_floatx80_init(0x3FFD, 0xB1641795CE3CA97B),
    make_floatx80_init(0x3FFE, 0xB30F63528917C80B),
    make_floatx80_init(0x3FFD, 0xB70475515D0F1C61),
    make_floatx80_init(0x3FFE, 0xB11FD3B80B11FD3C),
    make_floatx80_init(0x3FFD, 0xBC952AFEEA3D13E1),
    make_floatx80_init(0x3FFE, 0xAF3ADDC680AF3ADE),
    make_floatx80_init(0x3FFD, 0xC2168ED0F458BA4A),
    make_floatx80_init(0x3FFE, 0xAD602B580AD602B6),
    make_floatx80_init(0x3FFD, 0xC788F439B3163BF1),
    make_floatx80_init(0x3FFE, 0xAB8F69E28359CD11),
    make_floatx80_init(0x3FFD, 0xCCECAC08BF04565D),
    make_floatx80_init(0x3FFE, 0xA9C84A47A07F5638),
    make_floatx80_init(0x3FFD, 0xD24204872DD85160),
    make_floatx80_init(0x3FFE, 0xA80A80A80A80A80B),
    make_floatx80_init(0x3FFD, 0xD78949923BC3588A),
    make_floatx80_init(0x3FFE, 0xA655C4392D7B73A8),
    make_floatx80_init(0x3FFD, 0xDCC2C4B49887DACC),
    make_floatx80_init(0x3FFE, 0xA4A9CF1D96833751),
    make_floatx80_init(0x3FFD, 0xE1EEBD3E6D6A6B9E),
    make_floatx80_init(0x3FFE, 0xA3065E3FAE7CD0E0),
    make_floatx80_init(0x3FFD, 0xE70D785C2F9F5BDC),
    make_floatx80_init(0x3FFE, 0xA16B312EA8FC377D),
    make_floatx80_init(0x3FFD, 0xEC1F392C5179F283),
    make_floatx80_init(0x3FFE, 0x9FD809FD809FD80A),
    make_floatx80_init(0x3FFD, 0xF12440D3E36130E6),
    make_floatx80_init(0x3FFE, 0x9E4CAD23DD5F3A20),
    make_floatx80_init(0x3FFD, 0xF61CCE92346600BB),
    make_floatx80_init(0x3FFE, 0x9CC8E160C3FB19B9),
    make_floatx80_init(0x3FFD, 0xFB091FD38145630A),
    make_floatx80_init(0x3FFE, 0x9B4C6F9EF03A3CAA),
    make_floatx80_init(0x3FFD, 0xFFE97042BFA4C2AD),
    make_floatx80_init(0x3FFE, 0x99D722DABDE58F06),
    make_floatx80_init(0x3FFE, 0x825EFCED49369330),
    make_floatx80_init(0x3FFE, 0x9868C809868C8098),
    make_floatx80_init(0x3FFE, 0x84C37A7AB9A905C9),
    make_floatx80_init(0x3FFE, 0x97012E025C04B809),
    make_floatx80_init(0x3FFE, 0x87224C2E8E645FB7),
    make_floatx80_init(0x3FFE, 0x95A02568095A0257),
    make_floatx80_init(0x3FFE, 0x897B8CAC9F7DE298),
    make_floatx80_init(0x3FFE, 0x9445809445809446),
    make_floatx80_init(0x3FFE, 0x8BCF55DEC4CD05FE),
    make_floatx80_init(0x3FFE, 0x92F113840497889C),
    make_floatx80_init(0x3FFE, 0x8E1DC0FB89E125E5),
    make_floatx80_init(0x3FFE, 0x91A2B3C4D5E6F809),
    make_floatx80_init(0x3FFE, 0x9066E68C955B6C9B),
    make_floatx80_init(0x3FFE, 0x905A38633E06C43B),
    make_floatx80_init(0x3FFE, 0x92AADE74C7BE59E0),
    make_floatx80_init(0x3FFE, 0x8F1779D9FDC3A219),
    make_floatx80_init(0x3FFE, 0x94E9BFF615845643),
    make_floatx80_init(0x3FFE, 0x8DDA520237694809),
    make_floatx80_init(0x3FFE, 0x9723A1B720134203),
    make_floatx80_init(0x3FFE, 0x8CA29C046514E023),
    make_floatx80_init(0x3FFE, 0x995899C890EB8990),
    make_floatx80_init(0x3FFE, 0x8B70344A139BC75A),
    make_floatx80_init(0x3FFE, 0x9B88BDAA3A3DAE2F),
    make_floatx80_init(0x3FFE, 0x8A42F8705669DB46),
    make_floatx80_init(0x3FFE, 0x9DB4224FFFE1157C),
    make_floatx80_init(0x3FFE, 0x891AC73AE9819B50),
    make_floatx80_init(0x3FFE, 0x9FDADC268B7A12DA),
    make_floatx80_init(0x3FFE, 0x87F78087F78087F8),
    make_floatx80_init(0x3FFE, 0xA1FCFF17CE733BD4),
    make_floatx80_init(0x3FFE, 0x86D905447A34ACC6),
    make_floatx80_init(0x3FFE, 0xA41A9E8F5446FB9F),
    make_floatx80_init(0x3FFE, 0x85BF37612CEE3C9B),
    make_floatx80_init(0x3FFE, 0xA633CD7E6771CD8B),
    make_floatx80_init(0x3FFE, 0x84A9F9C8084A9F9D),
    make_floatx80_init(0x3FFE, 0xA8489E600B435A5E),
    make_floatx80_init(0x3FFE, 0x839930523FBE3368),
    make_floatx80_init(0x3FFE, 0xAA59233CCCA4BD49),
    make_floatx80_init(0x3FFE, 0x828CBFBEB9A020A3),
    make_floatx80_init(0x3FFE, 0xAC656DAE6BCC4985),
    make_floatx80_init(0x3FFE, 0x81848DA8FAF0D277),
    make_floatx80_init(0x3FFE, 0xAE6D8EE360BB2468),
    make_floatx80_init(0x3FFE, 0x8080808080808081),
    make_floatx80_init(0x3FFE, 0xB07197A23C46C654)
};

static const floatx80 exp_tbl[64] = {
    make_floatx80_init(0x3FFF, 0x8000000000000000),
    make_floatx80_init(0x3FFF, 0x8164D1F3BC030774),
    make_floatx80_init(0x3FFF, 0x82CD8698AC2BA1D8),
    make_floatx80_init(0x3FFF, 0x843A28C3ACDE4048),
    make_floatx80_init(0x3FFF, 0x85AAC367CC487B14),
    make_floatx80_init(0x3FFF, 0x871F61969E8D1010),
    make_floatx80_init(0x3FFF, 0x88980E8092DA8528),
    make_floatx80_init(0x3FFF, 0x8A14D575496EFD9C),
    make_floatx80_init(0x3FFF, 0x8B95C1E3EA8BD6E8),
    make_floatx80_init(0x3FFF, 0x8D1ADF5B7E5BA9E4),
    make_floatx80_init(0x3FFF, 0x8EA4398B45CD53C0),
    make_floatx80_init(0x3FFF, 0x9031DC431466B1DC),
    make_floatx80_init(0x3FFF, 0x91C3D373AB11C338),
    make_floatx80_init(0x3FFF, 0x935A2B2F13E6E92C),
    make_floatx80_init(0x3FFF, 0x94F4EFA8FEF70960),
    make_floatx80_init(0x3FFF, 0x96942D3720185A00),
    make_floatx80_init(0x3FFF, 0x9837F0518DB8A970),
    make_floatx80_init(0x3FFF, 0x99E0459320B7FA64),
    make_floatx80_init(0x3FFF, 0x9B8D39B9D54E5538),
    make_floatx80_init(0x3FFF, 0x9D3ED9A72CFFB750),
    make_floatx80_init(0x3FFF, 0x9EF5326091A111AC),
    make_floatx80_init(0x3FFF, 0xA0B0510FB9714FC4),
    make_floatx80_init(0x3FFF, 0xA27043030C496818),
    make_floatx80_init(0x3FFF, 0xA43515AE09E680A0),
    make_floatx80_init(0x3FFF, 0xA5FED6A9B15138EC),
    make_floatx80_init(0x3FFF, 0xA7CD93B4E9653568),
    make_floatx80_init(0x3FFF, 0xA9A15AB4EA7C0EF8),
    make_floatx80_init(0x3FFF, 0xAB7A39B5A93ED338),
    make_floatx80_init(0x3FFF, 0xAD583EEA42A14AC8),
    make_floatx80_init(0x3FFF, 0xAF3B78AD690A4374),
    make_floatx80_init(0x3FFF, 0xB123F581D2AC2590),
    make_floatx80_init(0x3FFF, 0xB311C412A9112488),
    make_floatx80_init(0x3FFF, 0xB504F333F9DE6484),
    make_floatx80_init(0x3FFF, 0xB6FD91E328D17790),
    make_floatx80_init(0x3FFF, 0xB8FBAF4762FB9EE8),
    make_floatx80_init(0x3FFF, 0xBAFF5AB2133E45FC),
    make_floatx80_init(0x3FFF, 0xBD08A39F580C36C0),
    make_floatx80_init(0x3FFF, 0xBF1799B67A731084),
    make_floatx80_init(0x3FFF, 0xC12C4CCA66709458),
    make_floatx80_init(0x3FFF, 0xC346CCDA24976408),
    make_floatx80_init(0x3FFF, 0xC5672A115506DADC),
    make_floatx80_init(0x3FFF, 0xC78D74C8ABB9B15C),
    make_floatx80_init(0x3FFF, 0xC9B9BD866E2F27A4),
    make_floatx80_init(0x3FFF, 0xCBEC14FEF2727C5C),
    make_floatx80_init(0x3FFF, 0xCE248C151F8480E4),
    make_floatx80_init(0x3FFF, 0xD06333DAEF2B2594),
    make_floatx80_init(0x3FFF, 0xD2A81D91F12AE45C),
    make_floatx80_init(0x3FFF, 0xD4F35AABCFEDFA20),
    make_floatx80_init(0x3FFF, 0xD744FCCAD69D6AF4),
    make_floatx80_init(0x3FFF, 0xD99D15C278AFD7B4),
    make_floatx80_init(0x3FFF, 0xDBFBB797DAF23754),
    make_floatx80_init(0x3FFF, 0xDE60F4825E0E9124),
    make_floatx80_init(0x3FFF, 0xE0CCDEEC2A94E110),
    make_floatx80_init(0x3FFF, 0xE33F8972BE8A5A50),
    make_floatx80_init(0x3FFF, 0xE5B906E77C8348A8),
    make_floatx80_init(0x3FFF, 0xE8396A503C4BDC68),
    make_floatx80_init(0x3FFF, 0xEAC0C6E7DD243930),
    make_floatx80_init(0x3FFF, 0xED4F301ED9942B84),
    make_floatx80_init(0x3FFF, 0xEFE4B99BDCDAF5CC),
    make_floatx80_init(0x3FFF, 0xF281773C59FFB138),
    make_floatx80_init(0x3FFF, 0xF5257D152486CC2C),
    make_floatx80_init(0x3FFF, 0xF7D0DF730AD13BB8),
    make_floatx80_init(0x3FFF, 0xFA83B2DB722A033C),
    make_floatx80_init(0x3FFF, 0xFD3E0C0CF486C174)
};

static const float32 exp_tbl2[64] = {
    const_float32(0x00000000),
    const_float32(0x9F841A9B),
    const_float32(0x9FC1D5B9),
    const_float32(0xA0728369),
    const_float32(0x1FC5C95C),
    const_float32(0x1EE85C9F),
    const_float32(0x9FA20729),
    const_float32(0xA07BF9AF),
    const_float32(0xA0020DCF),
    const_float32(0x205A63DA),
    const_float32(0x1EB70051),
    const_float32(0x1F6EB029),
    const_float32(0xA0781494),
    const_float32(0x9EB319B0),
    const_float32(0x2017457D),
    const_float32(0x1F11D537),
    const_float32(0x9FB952DD),
    const_float32(0x1FE43087),
    const_float32(0x1FA2A818),
    const_float32(0x1FDE494D),
    const_float32(0x20504890),
    const_float32(0xA073691C),
    const_float32(0x1F9B7A05),
    const_float32(0xA0797126),
    const_float32(0xA071A140),
    const_float32(0x204F62DA),
    const_float32(0x1F283C4A),
    const_float32(0x9F9A7FDC),
    const_float32(0xA05B3FAC),
    const_float32(0x1FDF2610),
    const_float32(0x9F705F90),
    const_float32(0x201F678A),
    const_float32(0x1F32FB13),
    const_float32(0x20038B30),
    const_float32(0x200DC3CC),
    const_float32(0x9F8B2AE6),
    const_float32(0xA02BBF70),
    const_float32(0xA00BF518),
    const_float32(0xA041DD41),
    const_float32(0x9FDF137B),
    const_float32(0x201F1568),
    const_float32(0x1FC13A2E),
    const_float32(0xA03F8F03),
    const_float32(0x1FF4907D),
    const_float32(0x9E6E53E4),
    const_float32(0x1FD6D45C),
    const_float32(0xA076EDB9),
    const_float32(0x9FA6DE21),
    const_float32(0x1EE69A2F),
    const_float32(0x207F439F),
    const_float32(0x201EC207),
    const_float32(0x9E8BE175),
    const_float32(0x20032C4B),
    const_float32(0x2004DFF5),
    const_float32(0x1E72F47A),
    const_float32(0x1F722F22),
    const_float32(0xA017E945),
    const_float32(0x1F401A5B),
    const_float32(0x9FB9A9E3),
    const_float32(0x20744C05),
    const_float32(0x1F773A19),
    const_float32(0x1FFE90D5),
    const_float32(0xA041ED22),
    const_float32(0x1F853F3A),
};

static const floatx80 exp2_tbl[64] = {
    make_floatx80_init(0x3FFF, 0x8000000000000000),
    make_floatx80_init(0x3FFF, 0x8164D1F3BC030773),
    make_floatx80_init(0x3FFF, 0x82CD8698AC2BA1D7),
    make_floatx80_init(0x3FFF, 0x843A28C3ACDE4046),
    make_floatx80_init(0x3FFF, 0x85AAC367CC487B15),
    make_floatx80_init(0x3FFF, 0x871F61969E8D1010),
    make_floatx80_init(0x3FFF, 0x88980E8092DA8527),
    make_floatx80_init(0x3FFF, 0x8A14D575496EFD9A),
    make_floatx80_init(0x3FFF, 0x8B95C1E3EA8BD6E7),
    make_floatx80_init(0x3FFF, 0x8D1ADF5B7E5BA9E6),
    make_floatx80_init(0x3FFF, 0x8EA4398B45CD53C0),
    make_floatx80_init(0x3FFF, 0x9031DC431466B1DC),
    make_floatx80_init(0x3FFF, 0x91C3D373AB11C336),
    make_floatx80_init(0x3FFF, 0x935A2B2F13E6E92C),
    make_floatx80_init(0x3FFF, 0x94F4EFA8FEF70961),
    make_floatx80_init(0x3FFF, 0x96942D3720185A00),
    make_floatx80_init(0x3FFF, 0x9837F0518DB8A96F),
    make_floatx80_init(0x3FFF, 0x99E0459320B7FA65),
    make_floatx80_init(0x3FFF, 0x9B8D39B9D54E5539),
    make_floatx80_init(0x3FFF, 0x9D3ED9A72CFFB751),
    make_floatx80_init(0x3FFF, 0x9EF5326091A111AE),
    make_floatx80_init(0x3FFF, 0xA0B0510FB9714FC2),
    make_floatx80_init(0x3FFF, 0xA27043030C496819),
    make_floatx80_init(0x3FFF, 0xA43515AE09E6809E),
    make_floatx80_init(0x3FFF, 0xA5FED6A9B15138EA),
    make_floatx80_init(0x3FFF, 0xA7CD93B4E965356A),
    make_floatx80_init(0x3FFF, 0xA9A15AB4EA7C0EF8),
    make_floatx80_init(0x3FFF, 0xAB7A39B5A93ED337),
    make_floatx80_init(0x3FFF, 0xAD583EEA42A14AC6),
    make_floatx80_init(0x3FFF, 0xAF3B78AD690A4375),
    make_floatx80_init(0x3FFF, 0xB123F581D2AC2590),
    make_floatx80_init(0x3FFF, 0xB311C412A9112489),
    make_floatx80_init(0x3FFF, 0xB504F333F9DE6484),
    make_floatx80_init(0x3FFF, 0xB6FD91E328D17791),
    make_floatx80_init(0x3FFF, 0xB8FBAF4762FB9EE9),
    make_floatx80_init(0x3FFF, 0xBAFF5AB2133E45FB),
    make_floatx80_init(0x3FFF, 0xBD08A39F580C36BF),
    make_floatx80_init(0x3FFF, 0xBF1799B67A731083),
    make_floatx80_init(0x3FFF, 0xC12C4CCA66709456),
    make_floatx80_init(0x3FFF, 0xC346CCDA24976407),
    make_floatx80_init(0x3FFF, 0xC5672A115506DADD),
    make_floatx80_init(0x3FFF, 0xC78D74C8ABB9B15D),
    make_floatx80_init(0x3FFF, 0xC9B9BD866E2F27A3),
    make_floatx80_init(0x3FFF, 0xCBEC14FEF2727C5D),
    make_floatx80_init(0x3FFF, 0xCE248C151F8480E4),
    make_floatx80_init(0x3FFF, 0xD06333DAEF2B2595),
    make_floatx80_init(0x3FFF, 0xD2A81D91F12AE45A),
    make_floatx80_init(0x3FFF, 0xD4F35AABCFEDFA1F),
    make_floatx80_init(0x3FFF, 0xD744FCCAD69D6AF4),
    make_floatx80_init(0x3FFF, 0xD99D15C278AFD7B6),
    make_floatx80_init(0x3FFF, 0xDBFBB797DAF23755),
    make_floatx80_init(0x3FFF, 0xDE60F4825E0E9124),
    make_floatx80_init(0x3FFF, 0xE0CCDEEC2A94E111),
    make_floatx80_init(0x3FFF, 0xE33F8972BE8A5A51),
    make_floatx80_init(0x3FFF, 0xE5B906E77C8348A8),
    make_floatx80_init(0x3FFF, 0xE8396A503C4BDC68),
    make_floatx80_init(0x3FFF, 0xEAC0C6E7DD24392F),
    make_floatx80_init(0x3FFF, 0xED4F301ED9942B84),
    make_floatx80_init(0x3FFF, 0xEFE4B99BDCDAF5CB),
    make_floatx80_init(0x3FFF, 0xF281773C59FFB13A),
    make_floatx80_init(0x3FFF, 0xF5257D152486CC2C),
    make_floatx80_init(0x3FFF, 0xF7D0DF730AD13BB9),
    make_floatx80_init(0x3FFF, 0xFA83B2DB722A033A),
    make_floatx80_init(0x3FFF, 0xFD3E0C0CF486C175)
};

static const uint32_t exp2_tbl2[64] = {
    0x3F738000, 0x3FBEF7CA, 0x3FBDF8A9, 0x3FBCD7C9,
    0xBFBDE8DA, 0x3FBDE85C, 0x3FBEBBF1, 0x3FBB80CA,
    0xBFBA8373, 0xBFBE9670, 0x3FBDB700, 0x3FBEEEB0,
    0x3FBBFD6D, 0xBFBDB319, 0x3FBDBA2B, 0x3FBE91D5,
    0x3FBE8D5A, 0xBFBCDE7B, 0xBFBEBAAF, 0xBFBD86DA,
    0xBFBEBEDD, 0x3FBCC96E, 0xBFBEC90B, 0x3FBBD1DB,
    0x3FBCE5EB, 0xBFBEC274, 0x3FBEA83C, 0x3FBECB00,
    0x3FBE9301, 0xBFBD8367, 0xBFBEF05F, 0x3FBDFB3C,
    0x3FBEB2FB, 0x3FBAE2CB, 0x3FBCDC3C, 0x3FBEE9AA,
    0xBFBEAEFD, 0xBFBCBF51, 0x3FBEF88A, 0x3FBD83B2,
    0x3FBDF8AB, 0xBFBDFB17, 0xBFBEFE3C, 0xBFBBB6F8,
    0xBFBCEE53, 0xBFBDA4AE, 0x3FBC9124, 0x3FBEB243,
    0x3FBDE69A, 0xBFB8BC61, 0x3FBDF610, 0xBFBD8BE1,
    0x3FBACB12, 0x3FBB9BFE, 0x3FBCF2F4, 0x3FBEF22F,
    0xBFBDBF4A, 0x3FBEC01A, 0x3FBE8CAC, 0xBFBCBB3F,
    0x3FBEF73A, 0xBFB8B795, 0x3FBEF84B, 0xBFBEF581
};

static const floatx80 pi_tbl[65] = {
    make_floatx80_init(0xC004, 0xC90FDAA22168C235),
    make_floatx80_init(0xC004, 0xC2C75BCD105D7C23),
    make_floatx80_init(0xC004, 0xBC7EDCF7FF523611),
    make_floatx80_init(0xC004, 0xB6365E22EE46F000),
    make_floatx80_init(0xC004, 0xAFEDDF4DDD3BA9EE),
    make_floatx80_init(0xC004, 0xA9A56078CC3063DD),
    make_floatx80_init(0xC004, 0xA35CE1A3BB251DCB),
    make_floatx80_init(0xC004, 0x9D1462CEAA19D7B9),
    make_floatx80_init(0xC004, 0x96CBE3F9990E91A8),
    make_floatx80_init(0xC004, 0x9083652488034B96),
    make_floatx80_init(0xC004, 0x8A3AE64F76F80584),
    make_floatx80_init(0xC004, 0x83F2677A65ECBF73),
    make_floatx80_init(0xC003, 0xFB53D14AA9C2F2C2),
    make_floatx80_init(0xC003, 0xEEC2D3A087AC669F),
    make_floatx80_init(0xC003, 0xE231D5F66595DA7B),
    make_floatx80_init(0xC003, 0xD5A0D84C437F4E58),
    make_floatx80_init(0xC003, 0xC90FDAA22168C235),
    make_floatx80_init(0xC003, 0xBC7EDCF7FF523611),
    make_floatx80_init(0xC003, 0xAFEDDF4DDD3BA9EE),
    make_floatx80_init(0xC003, 0xA35CE1A3BB251DCB),
    make_floatx80_init(0xC003, 0x96CBE3F9990E91A8),
    make_floatx80_init(0xC003, 0x8A3AE64F76F80584),
    make_floatx80_init(0xC002, 0xFB53D14AA9C2F2C2),
    make_floatx80_init(0xC002, 0xE231D5F66595DA7B),
    make_floatx80_init(0xC002, 0xC90FDAA22168C235),
    make_floatx80_init(0xC002, 0xAFEDDF4DDD3BA9EE),
    make_floatx80_init(0xC002, 0x96CBE3F9990E91A8),
    make_floatx80_init(0xC001, 0xFB53D14AA9C2F2C2),
    make_floatx80_init(0xC001, 0xC90FDAA22168C235),
    make_floatx80_init(0xC001, 0x96CBE3F9990E91A8),
    make_floatx80_init(0xC000, 0xC90FDAA22168C235),
    make_floatx80_init(0xBFFF, 0xC90FDAA22168C235),
    make_floatx80_init(0x0000, 0x0000000000000000),
    make_floatx80_init(0x3FFF, 0xC90FDAA22168C235),
    make_floatx80_init(0x4000, 0xC90FDAA22168C235),
    make_floatx80_init(0x4001, 0x96CBE3F9990E91A8),
    make_floatx80_init(0x4001, 0xC90FDAA22168C235),
    make_floatx80_init(0x4001, 0xFB53D14AA9C2F2C2),
    make_floatx80_init(0x4002, 0x96CBE3F9990E91A8),
    make_floatx80_init(0x4002, 0xAFEDDF4DDD3BA9EE),
    make_floatx80_init(0x4002, 0xC90FDAA22168C235),
    make_floatx80_init(0x4002, 0xE231D5F66595DA7B),
    make_floatx80_init(0x4002, 0xFB53D14AA9C2F2C2),
    make_floatx80_init(0x4003, 0x8A3AE64F76F80584),
    make_floatx80_init(0x4003, 0x96CBE3F9990E91A8),
    make_floatx80_init(0x4003, 0xA35CE1A3BB251DCB),
    make_floatx80_init(0x4003, 0xAFEDDF4DDD3BA9EE),
    make_floatx80_init(0x4003, 0xBC7EDCF7FF523611),
    make_floatx80_init(0x4003, 0xC90FDAA22168C235),
    make_floatx80_init(0x4003, 0xD5A0D84C437F4E58),
    make_floatx80_init(0x4003, 0xE231D5F66595DA7B),
    make_floatx80_init(0x4003, 0xEEC2D3A087AC669F),
    make_floatx80_init(0x4003, 0xFB53D14AA9C2F2C2),
    make_floatx80_init(0x4004, 0x83F2677A65ECBF73),
    make_floatx80_init(0x4004, 0x8A3AE64F76F80584),
    make_floatx80_init(0x4004, 0x9083652488034B96),
    make_floatx80_init(0x4004, 0x96CBE3F9990E91A8),
    make_floatx80_init(0x4004, 0x9D1462CEAA19D7B9),
    make_floatx80_init(0x4004, 0xA35CE1A3BB251DCB),
    make_floatx80_init(0x4004, 0xA9A56078CC3063DD),
    make_floatx80_init(0x4004, 0xAFEDDF4DDD3BA9EE),
    make_floatx80_init(0x4004, 0xB6365E22EE46F000),
    make_floatx80_init(0x4004, 0xBC7EDCF7FF523611),
    make_floatx80_init(0x4004, 0xC2C75BCD105D7C23),
    make_floatx80_init(0x4004, 0xC90FDAA22168C235)
};

static const float32 pi_tbl2[65] = {
    const_float32(0x21800000),
    const_float32(0xA0D00000),
    const_float32(0xA1E80000),
    const_float32(0x21480000),
    const_float32(0xA1200000),
    const_float32(0x21FC0000),
    const_float32(0x21100000),
    const_float32(0xA1580000),
    const_float32(0x21E00000),
    const_float32(0x20B00000),
    const_float32(0xA1880000),
    const_float32(0x21C40000),
    const_float32(0x20000000),
    const_float32(0x21380000),
    const_float32(0xA1300000),
    const_float32(0x9FC00000),
    const_float32(0x21000000),
    const_float32(0xA1680000),
    const_float32(0xA0A00000),
    const_float32(0x20900000),
    const_float32(0x21600000),
    const_float32(0xA1080000),
    const_float32(0x1F800000),
    const_float32(0xA0B00000),
    const_float32(0x20800000),
    const_float32(0xA0200000),
    const_float32(0x20E00000),
    const_float32(0x1F000000),
    const_float32(0x20000000),
    const_float32(0x20600000),
    const_float32(0x1F800000),
    const_float32(0x1F000000),
    const_float32(0x00000000),
    const_float32(0x9F000000),
    const_float32(0x9F800000),
    const_float32(0xA0600000),
    const_float32(0xA0000000),
    const_float32(0x9F000000),
    const_float32(0xA0E00000),
    const_float32(0x20200000),
    const_float32(0xA0800000),
    const_float32(0x20B00000),
    const_float32(0x9F800000),
    const_float32(0x21080000),
    const_float32(0xA1600000),
    const_float32(0xA0900000),
    const_float32(0x20A00000),
    const_float32(0x21680000),
    const_float32(0xA1000000),
    const_float32(0x1FC00000),
    const_float32(0x21300000),
    const_float32(0xA1380000),
    const_float32(0xA0000000),
    const_float32(0xA1C40000),
    const_float32(0x21880000),
    const_float32(0xA0B00000),
    const_float32(0xA1E00000),
    const_float32(0x21580000),
    const_float32(0xA1100000),
    const_float32(0xA1FC0000),
    const_float32(0x21200000),
    const_float32(0xA1480000),
    const_float32(0x21E80000),
    const_float32(0x20D00000),
    const_float32(0xA1800000),
};

static const floatx80 atan_tbl[128] = {
    make_floatx80_init(0x3FFB, 0x83D152C5060B7A51),
    make_floatx80_init(0x3FFB, 0x8BC8544565498B8B),
    make_floatx80_init(0x3FFB, 0x93BE406017626B0D),
    make_floatx80_init(0x3FFB, 0x9BB3078D35AEC202),
    make_floatx80_init(0x3FFB, 0xA3A69A525DDCE7DE),
    make_floatx80_init(0x3FFB, 0xAB98E94362765619),
    make_floatx80_init(0x3FFB, 0xB389E502F9C59862),
    make_floatx80_init(0x3FFB, 0xBB797E436B09E6FB),
    make_floatx80_init(0x3FFB, 0xC367A5C739E5F446),
    make_floatx80_init(0x3FFB, 0xCB544C61CFF7D5C6),
    make_floatx80_init(0x3FFB, 0xD33F62F82488533E),
    make_floatx80_init(0x3FFB, 0xDB28DA8162404C77),
    make_floatx80_init(0x3FFB, 0xE310A4078AD34F18),
    make_floatx80_init(0x3FFB, 0xEAF6B0A8188EE1EB),
    make_floatx80_init(0x3FFB, 0xF2DAF1949DBE79D5),
    make_floatx80_init(0x3FFB, 0xFABD581361D47E3E),
    make_floatx80_init(0x3FFC, 0x8346AC210959ECC4),
    make_floatx80_init(0x3FFC, 0x8B232A08304282D8),
    make_floatx80_init(0x3FFC, 0x92FB70B8D29AE2F9),
    make_floatx80_init(0x3FFC, 0x9ACF476F5CCD1CB4),
    make_floatx80_init(0x3FFC, 0xA29E76304954F23F),
    make_floatx80_init(0x3FFC, 0xAA68C5D08AB85230),
    make_floatx80_init(0x3FFC, 0xB22DFFFD9D539F83),
    make_floatx80_init(0x3FFC, 0xB9EDEF453E900EA5),
    make_floatx80_init(0x3FFC, 0xC1A85F1CC75E3EA5),
    make_floatx80_init(0x3FFC, 0xC95D1BE828138DE6),
    make_floatx80_init(0x3FFC, 0xD10BF300840D2DE4),
    make_floatx80_init(0x3FFC, 0xD8B4B2BA6BC05E7A),
    make_floatx80_init(0x3FFC, 0xE0572A6BB42335F6),
    make_floatx80_init(0x3FFC, 0xE7F32A70EA9CAA8F),
    make_floatx80_init(0x3FFC, 0xEF88843264ECEFAA),
    make_floatx80_init(0x3FFC, 0xF7170A28ECC06666),
    make_floatx80_init(0x3FFD, 0x812FD288332DAD32),
    make_floatx80_init(0x3FFD, 0x88A8D1B1218E4D64),
    make_floatx80_init(0x3FFD, 0x9012AB3F23E4AEE8),
    make_floatx80_init(0x3FFD, 0x976CC3D411E7F1B9),
    make_floatx80_init(0x3FFD, 0x9EB689493889A227),
    make_floatx80_init(0x3FFD, 0xA5EF72C34487361B),
    make_floatx80_init(0x3FFD, 0xAD1700BAF07A7227),
    make_floatx80_init(0x3FFD, 0xB42CBCFAFD37EFB7),
    make_floatx80_init(0x3FFD, 0xBB303A940BA80F89),
    make_floatx80_init(0x3FFD, 0xC22115C6FCAEBBAF),
    make_floatx80_init(0x3FFD, 0xC8FEF3E686331221),
    make_floatx80_init(0x3FFD, 0xCFC98330B4000C70),
    make_floatx80_init(0x3FFD, 0xD6807AA1102C5BF9),
    make_floatx80_init(0x3FFD, 0xDD2399BC31252AA3),
    make_floatx80_init(0x3FFD, 0xE3B2A8556B8FC517),
    make_floatx80_init(0x3FFD, 0xEA2D764F64315989),
    make_floatx80_init(0x3FFD, 0xF3BF5BF8BAD1A21D),
    make_floatx80_init(0x3FFE, 0x801CE39E0D205C9A),
    make_floatx80_init(0x3FFE, 0x8630A2DADA1ED066),
    make_floatx80_init(0x3FFE, 0x8C1AD445F3E09B8C),
    make_floatx80_init(0x3FFE, 0x91DB8F1664F350E2),
    make_floatx80_init(0x3FFE, 0x97731420365E538C),
    make_floatx80_init(0x3FFE, 0x9CE1C8E6A0B8CDBA),
    make_floatx80_init(0x3FFE, 0xA22832DBCADAAE09),
    make_floatx80_init(0x3FFE, 0xA746F2DDB7602294),
    make_floatx80_init(0x3FFE, 0xAC3EC0FB997DD6A2),
    make_floatx80_init(0x3FFE, 0xB110688AEBDC6F6A),
    make_floatx80_init(0x3FFE, 0xB5BCC49059ECC4B0),
    make_floatx80_init(0x3FFE, 0xBA44BC7DD470782F),
    make_floatx80_init(0x3FFE, 0xBEA94144FD049AAC),
    make_floatx80_init(0x3FFE, 0xC2EB4ABB661628B6),
    make_floatx80_init(0x3FFE, 0xC70BD54CE602EE14),
    make_floatx80_init(0x3FFE, 0xCD000549ADEC7159),
    make_floatx80_init(0x3FFE, 0xD48457D2D8EA4EA3),
    make_floatx80_init(0x3FFE, 0xDB948DA712DECE3B),
    make_floatx80_init(0x3FFE, 0xE23855F969E8096A),
    make_floatx80_init(0x3FFE, 0xE8771129C4353259),
    make_floatx80_init(0x3FFE, 0xEE57C16E0D379C0D),
    make_floatx80_init(0x3FFE, 0xF3E10211A87C3779),
    make_floatx80_init(0x3FFE, 0xF919039D758B8D41),
    make_floatx80_init(0x3FFE, 0xFE058B8F64935FB3),
    make_floatx80_init(0x3FFF, 0x8155FB497B685D04),
    make_floatx80_init(0x3FFF, 0x83889E3549D108E1),
    make_floatx80_init(0x3FFF, 0x859CFA76511D724B),
    make_floatx80_init(0x3FFF, 0x87952ECFFF8131E7),
    make_floatx80_init(0x3FFF, 0x89732FD19557641B),
    make_floatx80_init(0x3FFF, 0x8B38CAD101932A35),
    make_floatx80_init(0x3FFF, 0x8CE7A8D8301EE6B5),
    make_floatx80_init(0x3FFF, 0x8F46A39E2EAE5281),
    make_floatx80_init(0x3FFF, 0x922DA7D791888487),
    make_floatx80_init(0x3FFF, 0x94D19FCBDEDF5241),
    make_floatx80_init(0x3FFF, 0x973AB94419D2A08B),
    make_floatx80_init(0x3FFF, 0x996FF00E08E10B96),
    make_floatx80_init(0x3FFF, 0x9B773F9512321DA7),
    make_floatx80_init(0x3FFF, 0x9D55CC320F935624),
    make_floatx80_init(0x3FFF, 0x9F100575006CC571),
    make_floatx80_init(0x3FFF, 0xA0A9C290D97CC06C),
    make_floatx80_init(0x3FFF, 0xA22659EBEBC0630A),
    make_floatx80_init(0x3FFF, 0xA388B4AFF6EF0EC9),
    make_floatx80_init(0x3FFF, 0xA4D35F1061D292C4),
    make_floatx80_init(0x3FFF, 0xA60895DCFBE3187E),
    make_floatx80_init(0x3FFF, 0xA72A51DC7367BEAC),
    make_floatx80_init(0x3FFF, 0xA83A51530956168F),
    make_floatx80_init(0x3FFF, 0xA93A20077539546E),
    make_floatx80_init(0x3FFF, 0xAA9E7245023B2605),
    make_floatx80_init(0x3FFF, 0xAC4C84BA6FE4D58F),
    make_floatx80_init(0x3FFF, 0xADCE4A4A606B9712),
    make_floatx80_init(0x3FFF, 0xAF2A2DCD8D263C9C),
    make_floatx80_init(0x3FFF, 0xB0656F81F22265C7),
    make_floatx80_init(0x3FFF, 0xB18465150F71496A),
    make_floatx80_init(0x3FFF, 0xB28AAA156F9ADA35),
    make_floatx80_init(0x3FFF, 0xB37B44FF3766B895),
    make_floatx80_init(0x3FFF, 0xB458C3DCE9630433),
    make_floatx80_init(0x3FFF, 0xB525529D562246BD),
    make_floatx80_init(0x3FFF, 0xB5E2CCA95F9D88CC),
    make_floatx80_init(0x3FFF, 0xB692CADA7ACA1ADA),
    make_floatx80_init(0x3FFF, 0xB736AEA7A6925838),
    make_floatx80_init(0x3FFF, 0xB7CFAB287E9F7B36),
    make_floatx80_init(0x3FFF, 0xB85ECC66CB219835),
    make_floatx80_init(0x3FFF, 0xB8E4FD5A20A593DA),
    make_floatx80_init(0x3FFF, 0xB99F41F64AFF9BB5),
    make_floatx80_init(0x3FFF, 0xBA7F1E17842BBE7B),
    make_floatx80_init(0x3FFF, 0xBB4712857637E17D),
    make_floatx80_init(0x3FFF, 0xBBFABE8A4788DF6F),
    make_floatx80_init(0x3FFF, 0xBC9D0FAD2B689D79),
    make_floatx80_init(0x3FFF, 0xBD306A39471ECD86),
    make_floatx80_init(0x3FFF, 0xBDB6C731856AF18A),
    make_floatx80_init(0x3FFF, 0xBE31CAC502E80D70),
    make_floatx80_init(0x3FFF, 0xBEA2D55CE33194E2),
    make_floatx80_init(0x3FFF, 0xBF0B10B7C03128F0),
    make_floatx80_init(0x3FFF, 0xBF6B7A18DACB778D),
    make_floatx80_init(0x3FFF, 0xBFC4EA4663FA18F6),
    make_floatx80_init(0x3FFF, 0xC0181BDE8B89A454),
    make_floatx80_init(0x3FFF, 0xC065B066CFBF6439),
    make_floatx80_init(0x3FFF, 0xC0AE345F56340AE6),
    make_floatx80_init(0x3FFF, 0xC0F222919CB9E6A7)
};
#endif
