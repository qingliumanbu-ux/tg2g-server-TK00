/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2012
Author: gongnn
Version:    3.0
Date:		2025-01-14
Description:碳效等级查询
**************************************************/
//框架用头文件
#include "stdafx.h"
// service入口
BM2F_ENTERACE(tksm20_inq)
//-EP_SYSTEM_HEAD_END

int f_tksm20_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int fetchRowCount = 0;

	CString begin_time("");
	CString end_time("");

	CString  sqlstr("");
	CString  sg_sign_div("");
	CString  heat_no_div("");
	CString  equ_no("");
	CString  gx_div("");
	CString  heat_no("");


	CDbCommand cmd_inq(conn);

	CModel ttksm02("TTKSM02");

	try
	{
		
		begin_time = bcls_rec->Tables[0].Rows[0]["BEGIN_TIME"].ToString().SubstringNE(0, 8);
		end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().SubstringNE(0, 8);
		//heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		sg_sign_div = bcls_rec->Tables[0].Rows[0]["SG_SIGN_DIV"].ToString();
		heat_no_div = bcls_rec->Tables[0].Rows[0]["HEAT_NO_DIV"].ToString();


		ttksm02.MergeFrom(bcls_rec->Tables[0].Rows[0]);


		if (sg_sign_div == "1") //按钢种区分
		{
			
			sqlstr = "select m.st_no,m.STEEL_RATE IRON_STEEL_RATIO,m.fg_rate,m.CO2_WT_UNIT, "
				"round(A0-A1*m.fg_rate,4) YA, round(B0-B1*m.fg_rate,4) YB, round(C0-C1*m.fg_rate,4) YC, round(D0-D1*m.fg_rate,4) YD, round(E0-E1*m.fg_rate,4) YE,"
				"(case when m.CO2_WT_UNIT<=(A0-A1*m.fg_rate) then 'A'"
				" when m.CO2_WT_UNIT>(A0-A1*m.fg_rate) and m.CO2_WT_UNIT<=(B0-B1*m.fg_rate) then 'B'"
				" when m.CO2_WT_UNIT>(B0-B1*m.fg_rate) and m.CO2_WT_UNIT<=(C0-C1*m.fg_rate) then 'C'"
				" when m.CO2_WT_UNIT>(C0-C1*m.fg_rate) and m.CO2_WT_UNIT<=(D0-D1*m.fg_rate) then 'D'"
				" when m.CO2_WT_UNIT>(D0-D1*m.fg_rate) and m.CO2_WT_UNIT<=(E0-E1*m.fg_rate) then 'E'"
				" else '' end) CO2_EFF_LEV from"
				" ( select t1.st_no,prod_wt,CO2_WT,ts_wt,fg_wt, "
				" decode(prod_wt, 0, 0, round(ts_wt / prod_wt, 4)) STEEL_RATE,"
				" decode(prod_wt, 0, 0, round(co2_wt / prod_wt, 4)) CO2_WT_UNIT,"
				" decode((ts_wt + fg_wt),0,0,round(t2.fg_wt / (t2.ts_wt + t2.fg_wt), 4)) FG_RATE "
				" from (select st_no, sum(prod_wt) prod_wt"
				" from ttksm01"
				" where 1=1"
				" and C_DIV = '2'"
				" and prod_time<=@end_time"
				" and prod_time>=@begin_time";

			if (ttksm02["ST_NO"].ToString().Trim() != "")
			{
				sqlstr = sqlstr + " and st_no=@st_no";
			}
				sqlstr = sqlstr + " group by st_no"
				") t1"
				" left join ( select st_no, sum(co2_wt) CO2_WT, sum(case when mat_code = 'TS0000' then wt else 0 end) ts_wt,sum(case when  mat_code in (select mat_code from ttk0001 where type_code1 = '1' and mat_code != 'TS0000') then wt else 0 end) fg_wt"	 //铁钢比
				" from ttksm02"
				" where 1=1"
				" and C_DIV = '2'"
				" and prod_time<=@end_time"
				" and prod_time>=@begin_time"
				" group by st_no"
				") t2 "
				" on t1.st_no = t2.st_no) m,"
				"(select sum(case when tt.co2_eff_lev='A' then tt.co2_eff_lev_vlue0 else 0 end) A0, "
				"sum(case when tt.co2_eff_lev='B' then tt.co2_eff_lev_vlue0 else 0 end) B0, "
				"sum(case when tt.co2_eff_lev='C' then tt.co2_eff_lev_vlue0 else 0 end) C0, "
				"sum(case when tt.co2_eff_lev='D' then tt.co2_eff_lev_vlue0 else 0 end) D0, "
				"sum(case when tt.co2_eff_lev='E' then tt.co2_eff_lev_vlue0 else 0 end) E0, "
				"sum(case when tt.co2_eff_lev='A' then tt.co2_gradient else 0 end) A1, "
				"sum(case when tt.co2_eff_lev='B' then tt.co2_gradient else 0 end) B1, "
				"sum(case when tt.co2_eff_lev='C' then tt.co2_gradient else 0 end) C1, "
				"sum(case when tt.co2_eff_lev='D' then tt.co2_gradient else 0 end) D1, "
				"sum(case when tt.co2_eff_lev='E' then tt.co2_gradient else 0 end) E1 "
				"from ttk0008 tt ) n"
				;
			
			    
		}
		else if (heat_no_div == "1") //按炉号区分
		{
			sqlstr = "select m.st_no,m.heat_no,m.STEEL_RATE IRON_STEEL_RATIO,m.fg_rate,m.CO2_WT_UNIT, "
				"round(A0-A1*m.fg_rate,4) YA, round(B0-B1*m.fg_rate,4) YB, round(C0-C1*m.fg_rate,4) YC, round(D0-D1*m.fg_rate,4) YD, round(E0-E1*m.fg_rate,4) YE,"
				"(case when m.CO2_WT_UNIT<=(A0-A1*m.fg_rate) then 'A'"
				" when m.CO2_WT_UNIT>(A0-A1*m.fg_rate) and m.CO2_WT_UNIT<=(B0-B1*m.fg_rate) then 'B'"
				" when m.CO2_WT_UNIT>(B0-B1*m.fg_rate) and m.CO2_WT_UNIT<=(C0-C1*m.fg_rate) then 'C'"
				" when m.CO2_WT_UNIT>(C0-C1*m.fg_rate) and m.CO2_WT_UNIT<=(D0-D1*m.fg_rate) then 'D'"
				" when m.CO2_WT_UNIT>(D0-D1*m.fg_rate) and m.CO2_WT_UNIT<=(E0-E1*m.fg_rate) then 'E'"
				" else '' end) CO2_EFF_LEV from"
				" ( select t1.heat_no,t1.st_no,prod_wt,CO2_WT,ts_wt,fg_wt, "
				" decode(prod_wt, 0, 0, round(ts_wt / prod_wt, 4)) STEEL_RATE,"
				" decode(prod_wt, 0, 0, round(co2_wt / prod_wt, 4)) CO2_WT_UNIT,"
				" decode((ts_wt + fg_wt),0,0,round(t2.fg_wt / (t2.ts_wt + t2.fg_wt), 4)) FG_RATE "
				" from (select st_no, heat_no, sum(prod_wt) prod_wt"
				" from ttksm01"
				" where 1=1"
				" and C_DIV = '2'"
				" and prod_time<=@end_time"
				" and prod_time>=@begin_time";
			if (ttksm02["HEAT_NO"].ToString().Trim() != "")
			{
				sqlstr = sqlstr + " and heat_no=@heat_no";
			}
			if (ttksm02["ST_NO"].ToString().Trim() != "")
			{
				sqlstr = sqlstr + " and st_no=@st_no";
			}


			   sqlstr = sqlstr + " group by st_no, heat_no"
				") t1"
				" left join ( select heat_no, sum(co2_wt) CO2_WT, sum(case when mat_code = 'TS0000' then wt else 0 end) ts_wt,sum(case when  mat_code in (select mat_code from ttk0001 where type_code1 = '1' and mat_code != 'TS0000') then wt else 0 end) fg_wt"	 //铁钢比
				" from ttksm02"
				" where 1=1"
				" and C_DIV = '2'"
				" and prod_time<=@end_time"
				" and prod_time>=@begin_time"
				" group by heat_no"
				") t2 "
				" on t1.heat_no = t2.heat_no) m,"
			    "(select sum(case when tt.co2_eff_lev='A' then tt.co2_eff_lev_vlue0 else 0 end) A0, "
				"sum(case when tt.co2_eff_lev='B' then tt.co2_eff_lev_vlue0 else 0 end) B0, "
				"sum(case when tt.co2_eff_lev='C' then tt.co2_eff_lev_vlue0 else 0 end) C0, "
				"sum(case when tt.co2_eff_lev='D' then tt.co2_eff_lev_vlue0 else 0 end) D0, "
				"sum(case when tt.co2_eff_lev='E' then tt.co2_eff_lev_vlue0 else 0 end) E0, "
				"sum(case when tt.co2_eff_lev='A' then tt.co2_gradient else 0 end) A1, "
				"sum(case when tt.co2_eff_lev='B' then tt.co2_gradient else 0 end) B1, "
				"sum(case when tt.co2_eff_lev='C' then tt.co2_gradient else 0 end) C1, "
				"sum(case when tt.co2_eff_lev='D' then tt.co2_gradient else 0 end) D1, "
				"sum(case when tt.co2_eff_lev='E' then tt.co2_gradient else 0 end) E1 "
				"from ttk0008 tt ) n"
				;
				
			
		}

		else
		{
			sqlstr = "select m.STEEL_RATE IRON_STEEL_RATIO,m.fg_rate,m.CO2_WT_UNIT, "
				"round(A0-A1*m.fg_rate,4) YA, round(B0-B1*m.fg_rate,4) YB, round(C0-C1*m.fg_rate,4) YC, round(D0-D1*m.fg_rate,4) YD, round(E0-E1*m.fg_rate,4) YE,"
				"(case when m.CO2_WT_UNIT<=(A0-A1*m.fg_rate) then 'A'"
				" when m.CO2_WT_UNIT>(A0-A1*m.fg_rate) and m.CO2_WT_UNIT<=(B0-B1*m.fg_rate) then 'B'"
				" when m.CO2_WT_UNIT>(B0-B1*m.fg_rate) and m.CO2_WT_UNIT<=(C0-C1*m.fg_rate) then 'C'"
				" when m.CO2_WT_UNIT>(C0-C1*m.fg_rate) and m.CO2_WT_UNIT<=(D0-D1*m.fg_rate) then 'D'"
				" when m.CO2_WT_UNIT>(D0-D1*m.fg_rate) and m.CO2_WT_UNIT<=(E0-E1*m.fg_rate) then 'E'"
				" else '' end) CO2_EFF_LEV from"
				" ( select prod_wt,CO2_WT,ts_wt,fg_wt, "
				" decode(prod_wt, 0, 0, round(ts_wt / prod_wt, 4)) STEEL_RATE,"
				" decode(prod_wt, 0, 0, round(co2_wt / prod_wt, 4)) CO2_WT_UNIT,"
				" decode((ts_wt + fg_wt),0,0,round(t2.fg_wt / (t2.ts_wt + t2.fg_wt), 4)) FG_RATE "
				" from (select sum(prod_wt) prod_wt"
				" from ttksm01"
				" where 1=1"
				" and C_DIV = '2'"
				" and prod_time<=@end_time"
				" and prod_time>=@begin_time"
				") t1"
				" left join ( select sum(co2_wt) CO2_WT, sum(case when mat_code = 'TS0000' then wt else 0 end) ts_wt,sum(case when  mat_code in (select mat_code from ttk0001 where type_code1 = '1' and mat_code != 'TS0000') then wt else 0 end) fg_wt"	 //铁钢比
				" from ttksm02"
				" where 1=1"
				" and C_DIV = '2'"
				" and prod_time<=@end_time"
				" and prod_time>=@begin_time"
				") t2 "
				" on 1=1) m,"
				"(select sum(case when tt.co2_eff_lev='A' then tt.co2_eff_lev_vlue0 else 0 end) A0, "
				"sum(case when tt.co2_eff_lev='B' then tt.co2_eff_lev_vlue0 else 0 end) B0, "
				"sum(case when tt.co2_eff_lev='C' then tt.co2_eff_lev_vlue0 else 0 end) C0, "
				"sum(case when tt.co2_eff_lev='D' then tt.co2_eff_lev_vlue0 else 0 end) D0, "
				"sum(case when tt.co2_eff_lev='E' then tt.co2_eff_lev_vlue0 else 0 end) E0, "
				"sum(case when tt.co2_eff_lev='A' then tt.co2_gradient else 0 end) A1, "
				"sum(case when tt.co2_eff_lev='B' then tt.co2_gradient else 0 end) B1, "
				"sum(case when tt.co2_eff_lev='C' then tt.co2_gradient else 0 end) C1, "
				"sum(case when tt.co2_eff_lev='D' then tt.co2_gradient else 0 end) D1, "
				"sum(case when tt.co2_eff_lev='E' then tt.co2_gradient else 0 end) E1 "
				"from ttk0008 tt ) n"
				;
		}
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		//cmd_inq.Parameters.Set("c_div", ttksm02["C_DIV"].ToString());
		cmd_inq.Parameters.Set("heat_no", ttksm02["HEAT_NO"].ToString());
		cmd_inq.Parameters.Set("st_no", ttksm02["ST_NO"].ToString());
		Log::Trace("", __FUNCTION__, "c_div", ttksm02["C_DIV"].ToString());
		//Log::Trace("", __FUNCTION__, "sg_sign_div", ttksm02["SG_SIGN_DIV"].ToString());
		//Log::Trace("", __FUNCTION__, "heat_no_div", ttksm02["HEAT_NO_DIV"].ToString());
		//Log::Trace("", __FUNCTION__, "heat_no", ttksm02["HEAT_NO"].ToString());
		//Log::Trace("", __FUNCTION__, "begin_time", begin_time);
		//Log::Trace("", __FUNCTION__, "end_time", end_time);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();



	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应

		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	s.flag = doFlag;
	bcls_ret->SetSYS(s);
	return doFlag;
}