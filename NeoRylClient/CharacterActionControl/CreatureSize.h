// CreatureSize.h: interface for the CCreatureSize class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CREATURESIZE_H__BBEA8409_2ADF_4341_B6EB_3D7628A334F9__INCLUDED_)
#define AFX_CREATURESIZE_H__BBEA8409_2ADF_4341_B6EB_3D7628A334F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCreatureSize  
{
	public:
		vector3 vecMonsterMinBox[200];
		vector3 vecMonsterMaxBox[200];

		CCreatureSize()
		{

			vecMonsterMinBox[1]=vector3(-35.0f, 0.0f, -55.0f);
			vecMonsterMaxBox[1]=vector3(35.0f, 0.0f, 65.0f);

			vecMonsterMinBox[2]=vector3(-42.0f, 0.0f, -66.0f);
			vecMonsterMaxBox[2]=vector3(42.0f, 0.0f, 78.0f);

			vecMonsterMinBox[3]=vector3(-49.0f, 0.0f, -77.0f);
			vecMonsterMaxBox[3]=vector3(49.0f, 0.0f, 91.0f);

			vecMonsterMinBox[4]=vector3(-49.0f, 0.0f, -77.0f);
			vecMonsterMaxBox[4]=vector3(49.0f, 0.0f, 91.0f);

			vecMonsterMinBox[5]=vector3(-49.0f, 0.0f, -77.0f);
			vecMonsterMaxBox[5]=vector3(49.0f, 0.0f, 91.0f);

			vecMonsterMinBox[6]=vector3(-49.0f, 0.0f, -77.0f);
			vecMonsterMaxBox[6]=vector3(49.0f, 0.0f, 91.0f);

			vecMonsterMinBox[7]=vector3(-56.0f, 0.0f, -88.0f);
			vecMonsterMaxBox[7]=vector3(56.0f, 0.0f, 104.0f);

			vecMonsterMinBox[8]=vector3(-56.0f, 0.0f, -88.0f);
			vecMonsterMaxBox[8]=vector3(56.0f, 0.0f, 104.0f);

			vecMonsterMinBox[9]=vector3(-32.0f, 0.0f, -40.0f);
			vecMonsterMaxBox[9]=vector3(32.0f, 0.0f, 40.0f);

			vecMonsterMinBox[10]=vector3(-45.0f, 0.0f, -35.0f);
			vecMonsterMaxBox[10]=vector3(35.0f, 0.0f, 55.0f);

			vecMonsterMinBox[11]=vector3(-40.0f, 0.0f, -25.0f);
			vecMonsterMaxBox[11]=vector3(40.0f, 0.0f, 25.0f);

			vecMonsterMinBox[12]=vector3(-40.0f, 0.0f, -55.0f);
			vecMonsterMaxBox[12]=vector3(30.0f, 0.0f, 20.0f);

			vecMonsterMinBox[13]=vector3(-40.0f, 0.0f, -55.0f);
			vecMonsterMaxBox[13]=vector3(30.0f, 0.0f, 20.0f);

			vecMonsterMinBox[14]=vector3(-49.5f, 0.0f, -38.5f);
			vecMonsterMaxBox[14]=vector3(38.5f, 0.0f, 60.5f);

			vecMonsterMinBox[15]=vector3(-44.0f, 0.0f, -27.5f);
			vecMonsterMaxBox[15]=vector3(44.0f, 0.0f, 27.5f);

			vecMonsterMinBox[16]=vector3(-44.0f, 0.0f, -60.5f);
			vecMonsterMaxBox[16]=vector3(33.0f, 0.0f, 22.0f);

			vecMonsterMinBox[17]=vector3(-44.0f, 0.0f, -60.5f);
			vecMonsterMaxBox[17]=vector3(33.0f, 0.0f, 22.0f);

			vecMonsterMinBox[18]=vector3(-54.0f, 0.0f, -42.0f);
			vecMonsterMaxBox[18]=vector3(42.0f, 0.0f, 66.0f);

			vecMonsterMinBox[19]=vector3(-48.0f, 0.0f, -30.0f);
			vecMonsterMaxBox[19]=vector3(48.0f, 0.0f, 30.0f);

			vecMonsterMinBox[20]=vector3(-50.0f, 0.0f, -25.0f);
			vecMonsterMaxBox[20]=vector3(65.0f, 0.0f, 155.0f);

			vecMonsterMinBox[21]=vector3(-48.0f, 0.0f, -66.0f);
			vecMonsterMaxBox[21]=vector3(36.0f, 0.0f, 24.0f);

			vecMonsterMinBox[22]=vector3(-48.0f, 0.0f, -66.0f);
			vecMonsterMaxBox[22]=vector3(36.0f, 0.0f, 24.0f);

			vecMonsterMinBox[23]=vector3(-30.0f, 0.0f, -15.0f);
			vecMonsterMaxBox[23]=vector3(35.0f, 0.0f, 150.0f);

			vecMonsterMinBox[24]=vector3(-25.0f, 0.0f, -20.0f);
			vecMonsterMaxBox[24]=vector3(25.0f, 0.0f, 35.0f);

			vecMonsterMinBox[25]=vector3(-20.0f, 0.0f, -70.0f);
			vecMonsterMaxBox[25]=vector3(15.0f, 0.0f, 55.0f);

			vecMonsterMinBox[26]=vector3(-95.0f, 0.0f, -80.0f);
			vecMonsterMaxBox[26]=vector3(95.0f, 0.0f, 100.0f);

			vecMonsterMinBox[27]=vector3(-36.0f, 0.0f, -18.0f);
			vecMonsterMaxBox[27]=vector3(42.0f, 0.0f, 180.0f);

			vecMonsterMinBox[28]=vector3(-35.0f, 0.0f, -17.5f);
			vecMonsterMaxBox[28]=vector3(24.5f, 0.0f, 45.5f);

			vecMonsterMinBox[29]=vector3(-30.0f, 0.0f, -24.0f);
			vecMonsterMaxBox[29]=vector3(30.0f, 0.0f, 42.0f);

			vecMonsterMinBox[30]=vector3(-24.0f, 0.0f, -84.0f);
			vecMonsterMaxBox[30]=vector3(18.0f, 0.0f, 66.0f);

			vecMonsterMinBox[31]=vector3(-142.5f, 0.0f, -120.0f);
			vecMonsterMaxBox[31]=vector3(142.5f, 0.0f, 150.0f);

			vecMonsterMinBox[32]=vector3(-50.0f, 0.0f, -25.0f);
			vecMonsterMaxBox[32]=vector3(35.0f, 0.0f, 65.0f);

			vecMonsterMinBox[33]=vector3(-190.0f, 0.0f, -160.0f);
			vecMonsterMaxBox[33]=vector3(190.0f, 0.0f, 200.0f);

			vecMonsterMinBox[34]=vector3(-60.0f, 0.0f, -30.0f);
			vecMonsterMaxBox[34]=vector3(42.0f, 0.0f, 78.0f);

			vecMonsterMinBox[35]=vector3(-120.0f, 0.0f, -115.0f);
			vecMonsterMaxBox[35]=vector3(95.0f, 0.0f, 120.0f);

			vecMonsterMinBox[36]=vector3(-90.0f, 0.0f, -120.0f);
			vecMonsterMaxBox[36]=vector3(90.0f, 0.0f, 80.0f);

			vecMonsterMinBox[37]=vector3(-45.0f, 0.0f, -165.0f);
			vecMonsterMaxBox[37]=vector3(55.0f, 0.0f, 80.0f);

			vecMonsterMinBox[38]=vector3(-45.0f, 0.0f, -180.0f);
			vecMonsterMaxBox[38]=vector3(50.0f, 0.0f, 90.0f);

			vecMonsterMinBox[39]=vector3(-45.0f, 0.0f, -180.0f);
			vecMonsterMaxBox[39]=vector3(50.0f, 0.0f, 90.0f);

			vecMonsterMinBox[40]=vector3(-35.0f, 0.0f, -28.0f);
			vecMonsterMaxBox[40]=vector3(35.0f, 0.0f, 49.0f);

			vecMonsterMinBox[41]=vector3(-60.0f, 0.0f, -30.0f);
			vecMonsterMaxBox[41]=vector3(42.0f, 0.0f, 78.0f);

			vecMonsterMinBox[42]=vector3(-44.0f, 0.0f, -55.0f);
			vecMonsterMaxBox[42]=vector3(44.0f, 0.0f, 55.0f);

			vecMonsterMinBox[43]=vector3(-78.0f, 0.0f, -72.0f);
			vecMonsterMaxBox[43]=vector3(60.0f, 0.0f, 78.0f);

			vecMonsterMinBox[44]=vector3(-108.0f, 0.0f, -144.0f);
			vecMonsterMaxBox[44]=vector3(108.0f, 0.0f, 96.0f);

			vecMonsterMinBox[45]=vector3(-54.0f, 0.0f, -198.0f);
			vecMonsterMaxBox[45]=vector3(66.0f, 0.0f, 96.0f);

			vecMonsterMinBox[46]=vector3(-54.0f, 0.0f, -216.0f);
			vecMonsterMaxBox[46]=vector3(60.0f, 0.0f, 108.0f);

			vecMonsterMinBox[47]=vector3(-54.0f, 0.0f, -216.0f);
			vecMonsterMaxBox[47]=vector3(60.0f, 0.0f, 108.0f);

			vecMonsterMinBox[48]=vector3(-70.0f, 0.0f, -35.0f);
			vecMonsterMaxBox[48]=vector3(49.0f, 0.0f, 91.0f);

			vecMonsterMinBox[49]=vector3(-168.0f, 0.0f, -161.0f);
			vecMonsterMaxBox[49]=vector3(133.0f, 0.0f, 168.0f);

			vecMonsterMinBox[50]=vector3(-20.0f, 0.0f, -45.0f);
			vecMonsterMaxBox[50]=vector3(20.0f, 0.0f, 140.0f);

			vecMonsterMinBox[51]=vector3(-55.0f, 0.0f, -200.0f);
			vecMonsterMaxBox[51]=vector3(45.0f, 0.0f, 60.0f);

			vecMonsterMinBox[52]=vector3(-55.0f, 0.0f, -200.0f);
			vecMonsterMaxBox[52]=vector3(45.0f, 0.0f, 60.0f);

			vecMonsterMinBox[53]=vector3(-55.0f, 0.0f, -200.0f);
			vecMonsterMaxBox[53]=vector3(45.0f, 0.0f, 60.0f);

			vecMonsterMinBox[54]=vector3(-20.0f, 0.0f, -45.0f);
			vecMonsterMaxBox[54]=vector3(20.0f, 0.0f, 140.0f);

			vecMonsterMinBox[55]=vector3(-96.0f, 0.0f, -90.0f);
			vecMonsterMaxBox[55]=vector3(96.0f, 0.0f, 108.0f);

			vecMonsterMinBox[56]=vector3(-55.0f, 0.0f, -200.0f);
			vecMonsterMaxBox[56]=vector3(45.0f, 0.0f, 60.0f);

			vecMonsterMinBox[57]=vector3(-55.0f, 0.0f, -200.0f);
			vecMonsterMaxBox[57]=vector3(45.0f, 0.0f, 60.0f);

			vecMonsterMinBox[58]=vector3(-55.0f, 0.0f, -200.0f);
			vecMonsterMaxBox[58]=vector3(45.0f, 0.0f, 60.0f);

			vecMonsterMinBox[59]=vector3(-20.0f, 0.0f, -45.0f);
			vecMonsterMaxBox[59]=vector3(20.0f, 0.0f, 145.0f);

			vecMonsterMinBox[60]=vector3(-28.0f, 0.0f, -63.0f);
			vecMonsterMaxBox[60]=vector3(28.0f, 0.0f, 203.0f);

			vecMonsterMinBox[61]=vector3(-60.0f, 0.0f, -75.0f);
			vecMonsterMaxBox[61]=vector3(60.0f, 0.0f, 75.0f);

			vecMonsterMinBox[62]=vector3(-126.0f, 0.0f, -168.0f);
			vecMonsterMaxBox[62]=vector3(126.0f, 0.0f, 112.0f);

			vecMonsterMinBox[63]=vector3(-63.0f, 0.0f, -231.0f);
			vecMonsterMaxBox[63]=vector3(77.0f, 0.0f, 112.0f);

			vecMonsterMinBox[64]=vector3(-63.0f, 0.0f, -252.0f);
			vecMonsterMaxBox[64]=vector3(70.0f, 0.0f, 126.0f);

			vecMonsterMinBox[65]=vector3(-63.0f,0.0f, -252.0f);
			vecMonsterMaxBox[65]=vector3(70.0f, 0.0f, 126.0f);

			vecMonsterMinBox[66]=vector3(-70.0f,0.0f, -20.0f);
			vecMonsterMaxBox[66]=vector3(70.0f, 0.0f, 70.0f);

			vecMonsterMinBox[67]=vector3(-70.0f,0.0f, -20.0f);
			vecMonsterMaxBox[67]=vector3(70.0f, 0.0f, 70.0f);

			vecMonsterMinBox[68]=vector3(-112.0f,0.0f, -105.0f);
			vecMonsterMaxBox[68]=vector3(112.0f, 0.0f, 126.0f);

			vecMonsterMinBox[69]=vector3(-70.0f,0.0f, -20.0f);
			vecMonsterMaxBox[69]=vector3(70.0f, 0.0f, 70.0f);

			vecMonsterMinBox[70]=vector3(-70.0f,0.0f, -20.0f);
			vecMonsterMaxBox[70]=vector3(70.0f, 0.0f, 70.0f);

			vecMonsterMinBox[71]=vector3(-119.0f,0.0f, -49.0f);
			vecMonsterMaxBox[71]=vector3(119.0f, 0.0f, 189.0f);

			vecMonsterMinBox[72]=vector3(-168.0f,0.0f, -184.0f);
			vecMonsterMaxBox[72]=vector3(88.0f, 0.0f, 192.0f);

			vecMonsterMinBox[73]=vector3(-25.0f,0.0f, -20.0f);
			vecMonsterMaxBox[73]=vector3(20.0f, 0.0f, 40.0f);

			vecMonsterMinBox[74]=vector3(-75.0f,0.0f, -110.0f);
			vecMonsterMaxBox[74]=vector3(60.0f, 0.0f, 90.0f);

			vecMonsterMinBox[75]=vector3(-75.0f,0.0f, -110.0f);
			vecMonsterMaxBox[75]=vector3(60.0f, 0.0f, 90.0f);

			vecMonsterMinBox[76]=vector3(-25.0f,0.0f, -20.0f);
			vecMonsterMaxBox[76]=vector3(20.0f, 0.0f, 40.0f);

			vecMonsterMinBox[77]=vector3(-65.0f,0.0f, -50.0f);
			vecMonsterMaxBox[77]=vector3(75.0f, 0.0f, 75.0f);

			vecMonsterMinBox[78]=vector3(-70.0f,0.0f, -90.0f);
			vecMonsterMaxBox[78]=vector3(30.0f, 0.0f, 45.0f);

			vecMonsterMinBox[79]=vector3(-65.0f,0.0f, -50.0f);
			vecMonsterMaxBox[79]=vector3(75.0f, 0.0f, 75.0f);

			vecMonsterMinBox[80]=vector3(-65.0f,0.0f, -50.0f);
			vecMonsterMaxBox[80]=vector3(75.0f, 0.0f, 75.0f);

			vecMonsterMinBox[81]=vector3(-80.0f,0.0f, -65.0f);
			vecMonsterMaxBox[81]=vector3(100.0f, 0.0f, 85.0f);

			vecMonsterMinBox[82]=vector3(-55.0f,0.0f, -45.0f);
			vecMonsterMaxBox[82]=vector3(55.0f, 0.0f, 70.0f);

			vecMonsterMinBox[83]=vector3(-55.0f,0.0f, -45.0f);
			vecMonsterMaxBox[83]=vector3(55.0f, 0.0f, 70.0f);

			vecMonsterMinBox[84]=vector3(-20.0f,0.0f, -70.0f);
			vecMonsterMaxBox[84]=vector3(15.0f, 0.0f, 55.0f);

			vecMonsterMinBox[85]=vector3(-58.5f,0.0f, -36.0f);
			vecMonsterMaxBox[85]=vector3(108.0f, 0.0f, 144.0f);

			vecMonsterMinBox[86]=vector3(-71.5f,0.0f, -44.0f);
			vecMonsterMaxBox[86]=vector3(132.0f, 0.0f, 176.0f);

			vecMonsterMinBox[87]=vector3(-24.0f,0.0f, -84.0f);
			vecMonsterMaxBox[87]=vector3(18.0f, 0.0f, 66.0f);

			vecMonsterMinBox[88]=vector3(-240.0f,0.0f, -230.0f);
			vecMonsterMaxBox[88]=vector3(190.0f, 0.0f, 240.0f);

			vecMonsterMinBox[89]=vector3(-22.5f,0.0f, -52.5f);
			vecMonsterMaxBox[89]=vector3(22.5f, 0.0f, 127.5f);

			vecMonsterMinBox[90]=vector3(-66.0f,0.0f, -11.0f);
			vecMonsterMaxBox[90]=vector3(82.5f, 0.0f, 66.0f);

			vecMonsterMinBox[91]=vector3(-78.0f,0.0f, -72.0f);
			vecMonsterMaxBox[91]=vector3(60.0f, 0.0f, 78.0f);

			vecMonsterMinBox[92]=vector3(-98.0f,0.0f, -91.0f);
			vecMonsterMaxBox[92]=vector3(147.0f, 0.0f, 140.0f);

			vecMonsterMinBox[93]=vector3(-55.0f,0.0f, -45.0f);
			vecMonsterMaxBox[93]=vector3(55.0f, 0.0f, 70.0f);

			vecMonsterMinBox[94]=vector3(-91.0f,0.0f, -56.0f);
			vecMonsterMaxBox[94]=vector3(168.0f, 0.0f, 224.0f);

			vecMonsterMinBox[95]=vector3(-150.0f,0.0f, -65.0f);
			vecMonsterMaxBox[95]=vector3(105.0f, 0.0f, 160.0f);

			vecMonsterMinBox[96]=vector3(-130.0f,0.0f, -80.0f);
			vecMonsterMaxBox[96]=vector3(240.0f, 0.0f, 320.0f);

			vecMonsterMinBox[97]=vector3(-45.0f,0.0f, -105.0f);
			vecMonsterMaxBox[97]=vector3(45.0f, 0.0f, 255.0f);

			vecMonsterMinBox[98]=vector3(-150.0f,0.0f, -65.0f);
			vecMonsterMaxBox[98]=vector3(105.0f, 0.0f, 160.0f);

			vecMonsterMinBox[99]=vector3(-130.0f,0.0f, -55.0f);
			vecMonsterMaxBox[99]=vector3(80.0f, 0.0f, 130.0f);

			vecMonsterMinBox[100]=vector3(-115.0f,0.0f, -55.0f);
			vecMonsterMaxBox[100]=vector3(80.0f, 0.0f, 75.0f);

			vecMonsterMinBox[101]=vector3(-180.0f,0.0f, -78.0f);
			vecMonsterMaxBox[101]=vector3(126.0f, 0.0f, 192.0f);

			vecMonsterMinBox[102]=vector3(-156.0f,0.0f, -66.0f);
			vecMonsterMaxBox[102]=vector3(96.0f, 0.0f, 156.0f);

			vecMonsterMinBox[103]=vector3(-96.0f,0.0f, -16.0f);
			vecMonsterMaxBox[103]=vector3(120.0f, 0.0f, 96.0f);

			vecMonsterMinBox[104]=vector3(-138.0f,0.0f, -66.0f);
			vecMonsterMaxBox[104]=vector3(96.0f, 0.0f, 90.0f);

			vecMonsterMinBox[105]=vector3(-50.0f,0.0f, -25.0f);
			vecMonsterMaxBox[105]=vector3(60.0f, 0.0f, 130.0f);

			vecMonsterMinBox[106]=vector3(-50.0f,0.0f, -20.0f);
			vecMonsterMaxBox[106]=vector3(45.0f, 0.0f, 40.0f);

			vecMonsterMinBox[107]=vector3(-25.0f,0.0f, -35.0f);
			vecMonsterMaxBox[107]=vector3(25.0f, 0.0f, 150.0f);

			vecMonsterMinBox[108]=vector3(-25.0f,0.0f, -35.0f);
			vecMonsterMaxBox[108]=vector3(25.0f, 0.0f, 150.0f);

			vecMonsterMinBox[109]=vector3(-30.0f,0.0f, -35.0f);
			vecMonsterMaxBox[109]=vector3(40.0f, 0.0f, 60.0f);

			vecMonsterMinBox[110]=vector3(-30.0f,0.0f, -35.0f);
			vecMonsterMaxBox[110]=vector3(40.0f, 0.0f, 60.0f);

			vecMonsterMinBox[111]=vector3(-60.0f,0.0f, -30.0f);
			vecMonsterMaxBox[111]=vector3(72.0f, 0.0f, 156.0f);

			vecMonsterMinBox[112]=vector3(-60.0f,0.0f, -24.0f);
			vecMonsterMaxBox[112]=vector3(54.0f, 0.0f, 48.0f);

			vecMonsterMinBox[113]=vector3(-210.0f,0.0f, -91.0f);
			vecMonsterMaxBox[113]=vector3(147.0f, 0.0f, 224.0f);

			vecMonsterMinBox[114]=vector3(-182.0f,0.0f, -77.0f);
			vecMonsterMaxBox[114]=vector3(112.0f, 0.0f, 182.0f);

			vecMonsterMinBox[115]=vector3(-161.0f,0.0f, -77.0f);
			vecMonsterMaxBox[115]=vector3(112.0f, 0.0f, 105.0f);

			vecMonsterMinBox[116]=vector3(-85.0f,0.0f, -110.0f);
			vecMonsterMaxBox[116]=vector3(85.0f, 0.0f, 230.0f);

			vecMonsterMinBox[117]=vector3(-36.0f,0.0f, -42.0f);
			vecMonsterMaxBox[117]=vector3(48.0f, 0.0f, 72.0f);

			vecMonsterMinBox[118]=vector3(-36.0f,0.0f, -42.0f);
			vecMonsterMaxBox[118]=vector3(48.0f, 0.0f, 72.0f);

			vecMonsterMinBox[119]=vector3(-25.0f,0.0f, -35.0f);
			vecMonsterMaxBox[119]=vector3(25.0f, 0.0f, 150.0f);

			vecMonsterMinBox[120]=vector3(-95.0f,0.0f, 0.0f);
			vecMonsterMaxBox[120]=vector3(95.0f, 0.0f, 165.0f);

			vecMonsterMinBox[121]=vector3(-114.0f,0.0f, 0.0f);
			vecMonsterMaxBox[121]=vector3(114.0f, 0.0f, 198.0f);

			vecMonsterMinBox[122]=vector3(-25.0f,0.0f, -20.0f);
			vecMonsterMaxBox[122]=vector3(20.0f, 0.0f, 40.0f);

			vecMonsterMinBox[123]=vector3(-25.0f,0.0f, -20.0f);
			vecMonsterMaxBox[123]=vector3(20.0f, 0.0f, 40.0f);

			vecMonsterMinBox[124]=vector3(-65.0f,0.0f, -50.0f);
			vecMonsterMaxBox[124]=vector3(75.0f, 0.0f, 75.0f);

			vecMonsterMinBox[125]=vector3(-85.0f,0.0f, -110.0f);
			vecMonsterMaxBox[125]=vector3(85.0f, 0.0f, 230.0f);

			vecMonsterMinBox[126]=vector3(-133.0f,0.0f, 0.0f);
			vecMonsterMaxBox[126]=vector3(133.0f, 0.0f, 231.0f);

			vecMonsterMinBox[127]=vector3(-70.0f,0.0f, -35.0f);
			vecMonsterMaxBox[127]=vector3(84.0f, 0.0f, 182.0f);

			vecMonsterMinBox[128]=vector3(-70.0f,0.0f, -28.0f);
			vecMonsterMaxBox[128]=vector3(63.0f, 0.0f, 56.0f);

			vecMonsterMinBox[129]=vector3(-42.0f,0.0f, -49.0f);
			vecMonsterMaxBox[129]=vector3(56.0f, 0.0f, 84.0f);

			vecMonsterMinBox[130]=vector3(-42.0f,0.0f, -49.0f);
			vecMonsterMaxBox[130]=vector3(56.0f, 0.0f, 84.0f);

			vecMonsterMinBox[131]=vector3(-80.0f,0.0f, -40.0f);
			vecMonsterMaxBox[131]=vector3(96.0f, 0.0f, 208.0f);

			vecMonsterMinBox[132]=vector3(-84.5f,0.0f, -78.0f);
			vecMonsterMaxBox[132]=vector3(65.0f, 0.0f, 84.5f);

			vecMonsterMinBox[133]=vector3(-104.0f,0.0f, -96.0f);
			vecMonsterMaxBox[133]=vector3(80.0f, 0.0f, 104.0f);

			vecMonsterMinBox[134]=vector3(-104.0f,0.0f, -96.0f);
			vecMonsterMaxBox[134]=vector3(80.0f, 0.0f, 104.0f);

			vecMonsterMinBox[135]=vector3(-104.0f,0.0f, -96.0f);
			vecMonsterMaxBox[135]=vector3(80.0f, 0.0f, 104.0f);

			vecMonsterMinBox[136]=vector3(-80.0f,0.0f, -70.0f);
			vecMonsterMaxBox[136]=vector3(70.0f, 0.0f, 145.0f);

			vecMonsterMinBox[137]=vector3(-80.0f,0.0f, -55.0f);
			vecMonsterMaxBox[137]=vector3(60.0f, 0.0f, 115.0f);

			vecMonsterMinBox[138]=vector3(-240.0f,0.0f, -262.5f);
			vecMonsterMaxBox[138]=vector3(405.0f, 0.0f, 225.0f);

			vecMonsterMinBox[139]=vector3(-250.0f,0.0f, -205.0f);
			vecMonsterMaxBox[139]=vector3(195.0f, 0.0f, 135.0f);

			vecMonsterMinBox[140]=vector3(-25.0f,0.0f, -25.0f);
			vecMonsterMaxBox[140]=vector3(25.0f, 0.0f, 25.0f);

			vecMonsterMinBox[141]=vector3(-145.0f,0.0f, -280.0f);
			vecMonsterMaxBox[141]=vector3(135.0f, 0.0f, 230.0f);

			vecMonsterMinBox[142]=vector3(-40.0f,0.0f, -90.0f);
			vecMonsterMaxBox[142]=vector3(40.0f, 0.0f, 280.0f);

			vecMonsterMinBox[143]=vector3(-40.0f,0.0f, -90.0f);
			vecMonsterMaxBox[143]=vector3(40.0f, 0.0f, 290.0f);

			vecMonsterMinBox[144]=vector3(-50.0f,0.0f, -70.0f);
			vecMonsterMaxBox[144]=vector3(50.0f, 0.0f, 300.0f);

			vecMonsterMinBox[145]=vector3(-50.0f,0.0f, -70.0f);
			vecMonsterMaxBox[145]=vector3(50.0f, 0.0f, 300.0f);

			vecMonsterMinBox[146]=vector3(-81.0f,0.0f, -108.0f);
			vecMonsterMaxBox[146]=vector3(81.0f, 0.0f, 72.0f);

			vecMonsterMinBox[147]=vector3(-40.5f,0.0f, -148.5f);
			vecMonsterMaxBox[147]=vector3(49.5f, 0.0f, 72.0f);

			vecMonsterMinBox[148]=vector3(-30.0f,0.0f, -37.5f);
			vecMonsterMaxBox[148]=vector3(30.0f, 0.0f, 37.5f);

			vecMonsterMinBox[149]=vector3(-45.0f,0.0f, -22.5f);
			vecMonsterMaxBox[149]=vector3(58.5f, 0.0f, 139.5f);

			vecMonsterMinBox[150]=vector3(-72.0f,0.0f, -67.5f);
			vecMonsterMaxBox[150]=vector3(72.0f, 0.0f, 81.0f);

			vecMonsterMinBox[151]=vector3(-49.5f,0.0f, -180.0f);
			vecMonsterMaxBox[151]=vector3(40.5f, 0.0f, 54.0f);

			vecMonsterMinBox[152]=vector3(-54.0f,0.0f, -9.0f);
			vecMonsterMaxBox[152]=vector3(67.5f, 0.0f, 54.0f);

			vecMonsterMinBox[153]=vector3(-84.0f,0.0f, -78.0f);
			vecMonsterMaxBox[153]=vector3(126.0f, 0.0f, 120.0f);

			vecMonsterMinBox[154]=vector3(-96.0f,0.0f, -84.0f);
			vecMonsterMaxBox[154]=vector3(84.0f, 0.0f, 174.0f);

			vecMonsterMinBox[155]=vector3(-96.0f,0.0f, -66.0f);
			vecMonsterMaxBox[155]=vector3(72.0f, 0.0f, 138.0f);

			vecMonsterMinBox[156]=vector3(-95.0f,0.0f, 0.0f);
			vecMonsterMaxBox[156]=vector3(95.0f, 0.0f, 165.0f);

			vecMonsterMinBox[157]=vector3(-90.0f,0.0f, -120.0f);
			vecMonsterMaxBox[157]=vector3(90.0f, 0.0f, 80.0f);

			vecMonsterMinBox[158]=vector3(-45.0f,0.0f, -165.0f);
			vecMonsterMaxBox[158]=vector3(55.0f, 0.0f, 80.0f);

			vecMonsterMinBox[159]=vector3(-45.0f,0.0f, -22.5f);
			vecMonsterMaxBox[159]=vector3(58.5f, 0.0f, 139.5f);

		}

		~CCreatureSize() { }
};

#endif // !defined(AFX_CREATURESIZE_H__BBEA8409_2ADF_4341_B6EB_3D7628A334F9__INCLUDED_)
