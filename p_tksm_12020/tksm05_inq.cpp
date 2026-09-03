/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    3.0
Date:		2023-09-22
Description:查询
**************************************************/
//框架用头文件
#include "stdafx.h"
// service入口
BM2F_ENTERACE(tksm05_inq)
//-EP_SYSTEM_HEAD_END

int f_tksm05_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int fetchRowCount = 0;

	CString begin_time("");
	CString end_time("");

	CString		mat_type = " ";

	CModel ttksm02("TTKSM02");


	CString  sqlstr("");
	CString  sqlstr_sub("");


	CDbCommand cmd_inq(conn);

	try
	{
		begin_time = bcls_rec->Tables[0].Rows[0]["BEGIN_TIME"].ToString().SubstringNE(0, 8);
		end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().SubstringNE(0, 8);
		ttksm02.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		
		sqlstr_sub = ",sum(case when  mat_code in ('59103','59100','59101','59102') then CO2_WT else 0 end) USE_POWER"  //电
			",sum(case when  mat_code in ('59103','59100','59101','59102') then WT else 0 end) USE2_POWER"
			",sum(case when  mat_code ='59400' then CO2_WT else 0 end) USE_O2"	  //氧气
			",sum(case when  mat_code ='59400' then WT else 0 end) USE2_O2"
			",sum(case when  mat_code ='59402' then CO2_WT else 0 end) USE_AR"	 //氩气
			",sum(case when  mat_code ='59402' then WT else 0 end) USE2_AR"
			",sum(case when  mat_code ='59401' then CO2_WT else 0 end) USE_N"	  //氮气
			",sum(case when  mat_code ='59401' then WT else 0 end) USE2_N"
			",sum(case when  mat_code ='58001' then CO2_WT else 0 end) USE_H2O"	  //回收蒸汽
			",sum(case when  mat_code ='58001' then WT else 0 end) USE2_H2O"
			",sum(case when  mat_code ='48081' then CO2_WT else 0 end) USE_GAS"	  //回收转炉煤气
			",sum(case when  mat_code ='48081' then WT else 0 end) USE2_GAS"
			",sum(case when  mat_code in ('49093','49280','49092','49094') then CO2_WT else 0 end) USE_NY9"	  //消耗煤气
			",sum(case when  mat_code in ('49093','49280','49092','49094') then WT else 0 end) USE2_NY9"
			",sum(case when  mat_code in ('59103','59100','59101','59102') and EQU_NO like 'B%'  then CO2_WT else 0 end) B_USE_POWER"  //电
			",sum(case when  mat_code in ('59103','59100','59101','59102') and EQU_NO like 'B%'  then WT else 0 end) B_USE2_POWER"
			",sum(case when  mat_code ='59400' and EQU_NO like 'B%'  then CO2_WT else 0 end) B_USE_O2"	  //氧气
			",sum(case when  mat_code ='59400' and EQU_NO like 'B%'  then WT else 0 end) B_USE2_O2"
			",sum(case when  mat_code ='59402' and EQU_NO like 'B%'  then CO2_WT else 0 end) B_USE_AR"	 //氩气
			",sum(case when  mat_code ='59402' and EQU_NO like 'B%'  then WT else 0 end) B_USE2_AR"
			",sum(case when  mat_code ='59401' and EQU_NO like 'B%'  then CO2_WT else 0 end) B_USE_N"	  //氮气
			",sum(case when  mat_code ='59401' and EQU_NO like 'B%'  then WT else 0 end) B_USE2_N"
			",sum(case when  mat_code ='58001' and EQU_NO like 'B%'  then CO2_WT else 0 end) B_USE_H2O"	  //回收蒸汽
			",sum(case when  mat_code ='58001' and EQU_NO like 'B%'  then WT else 0 end) B_USE2_H2O"
			",sum(case when  mat_code ='48081' and EQU_NO like 'B%'  then CO2_WT else 0 end) B_USE_GAS"	  //回收转炉煤气
			",sum(case when  mat_code ='48081' and EQU_NO like 'B%'  then WT else 0 end) B_USE2_GAS"
			",sum(case when  mat_code in ('49093','49280','49092','49094') and EQU_NO like 'B%'  then CO2_WT else 0 end) B_USE_NY9"	  //消耗煤气
			",sum(case when  mat_code in ('49093','49280','49092','49094') and EQU_NO like 'B%'  then WT else 0 end) B_USE2_NY9"
			",sum(case when  mat_code in ('59103','59100','59101','59102') and EQU_NO like 'E%'  then CO2_WT else 0 end) E_USE_POWER"  //电
			",sum(case when  mat_code in ('59103','59100','59101','59102') and EQU_NO like 'E%'  then WT else 0 end) E_USE2_POWER"
			",sum(case when  mat_code ='59400' and EQU_NO like 'E%'  then CO2_WT else 0 end) E_USE_O2"	  //氧气
			",sum(case when  mat_code ='59400' and EQU_NO like 'E%'  then WT else 0 end) E_USE2_O2"
			",sum(case when  mat_code ='59402' and EQU_NO like 'E%'  then CO2_WT else 0 end) E_USE_AR"	 //氩气
			",sum(case when  mat_code ='59402' and EQU_NO like 'E%'  then WT else 0 end) E_USE2_AR"
			",sum(case when  mat_code ='59401' and EQU_NO like 'E%'  then CO2_WT else 0 end) E_USE_N"	  //氮气
			",sum(case when  mat_code ='59401' and EQU_NO like 'E%'  then WT else 0 end) E_USE2_N"
			",sum(case when  mat_code ='58001' and EQU_NO like 'E%'  then CO2_WT else 0 end) E_USE_H2O"	  //回收蒸汽
			",sum(case when  mat_code ='58001' and EQU_NO like 'E%'  then WT else 0 end) E_USE2_H2O"
			",sum(case when  mat_code ='48081' and EQU_NO like 'E%'  then CO2_WT else 0 end) E_USE_GAS"	  //回收转炉煤气
			",sum(case when  mat_code ='48081' and EQU_NO like 'E%'  then WT else 0 end) E_USE2_GAS"
			",sum(case when  mat_code in ('49093','49280','49092','49094') and EQU_NO like 'E%'  then CO2_WT else 0 end) E_USE_NY9"	  //消耗煤气
			",sum(case when  mat_code in ('49093','49280','49092','49094') and EQU_NO like 'E%'  then WT else 0 end) E_USE2_NY9"
			",sum(case when  mat_code in ('59103','59100','59101','59102') and EQU_NO like 'A%'  then CO2_WT else 0 end) A_USE_POWER"  //电
			",sum(case when  mat_code in ('59103','59100','59101','59102') and EQU_NO like 'A%'  then WT else 0 end) A_USE2_POWER"
			",sum(case when  mat_code ='59400' and EQU_NO like 'A%'  then CO2_WT else 0 end) A_USE_O2"	  //氧气
			",sum(case when  mat_code ='59400' and EQU_NO like 'A%'  then WT else 0 end) A_USE2_O2"
			",sum(case when  mat_code ='59402' and EQU_NO like 'A%'  then CO2_WT else 0 end) A_USE_AR"	 //氩气
			",sum(case when  mat_code ='59402' and EQU_NO like 'A%'  then WT else 0 end) A_USE2_AR"
			",sum(case when  mat_code ='59401' and EQU_NO like 'A%'  then CO2_WT else 0 end) A_USE_N"	  //氮气
			",sum(case when  mat_code ='59401' and EQU_NO like 'A%'  then WT else 0 end) A_USE2_N"
			",sum(case when  mat_code ='58001' and EQU_NO like 'A%'  then CO2_WT else 0 end) A_USE_H2O"	  //回收蒸汽
			",sum(case when  mat_code ='58001' and EQU_NO like 'A%'  then WT else 0 end) A_USE2_H2O"
			",sum(case when  mat_code ='48081' and EQU_NO like 'A%'  then CO2_WT else 0 end) A_USE_GAS"	  //回收转炉煤气
			",sum(case when  mat_code ='48081' and EQU_NO like 'A%'  then WT else 0 end) A_USE2_GAS"
			",sum(case when  mat_code in ('49093','49280','49092','49094') and EQU_NO like 'A%'  then CO2_WT else 0 end) A_USE_NY9"	  //消耗煤气
			",sum(case when  mat_code in ('49093','49280','49092','49094') and EQU_NO like 'A%'  then WT else 0 end) A_USE2_NY9"
			",sum(case when  mat_code in ('59103','59100','59101','59102') and EQU_NO like 'F%'  then CO2_WT else 0 end) F_USE_POWER"  //电
			",sum(case when  mat_code in ('59103','59100','59101','59102') and EQU_NO like 'F%'  then WT else 0 end) F_USE2_POWER"
			",sum(case when  mat_code ='59400' and EQU_NO like 'F%'  then CO2_WT else 0 end) F_USE_O2"	  //氧气
			",sum(case when  mat_code ='59400' and EQU_NO like 'F%'  then WT else 0 end) F_USE2_O2"
			",sum(case when  mat_code ='59402' and EQU_NO like 'F%'  then CO2_WT else 0 end) F_USE_AR"	 //氩气
			",sum(case when  mat_code ='59402' and EQU_NO like 'F%'  then WT else 0 end) F_USE2_AR"
			",sum(case when  mat_code ='59401' and EQU_NO like 'F%'  then CO2_WT else 0 end) F_USE_N"	  //氮气
			",sum(case when  mat_code ='59401' and EQU_NO like 'F%'  then WT else 0 end) F_USE2_N"
			",sum(case when  mat_code ='58001' and EQU_NO like 'F%'  then CO2_WT else 0 end) F_USE_H2O"	  //回收蒸汽
			",sum(case when  mat_code ='58001' and EQU_NO like 'F%'  then WT else 0 end) F_USE2_H2O"
			",sum(case when  mat_code ='48081' and EQU_NO like 'F%'  then CO2_WT else 0 end) F_USE_GAS"	  //回收转炉煤气
			",sum(case when  mat_code ='48081' and EQU_NO like 'F%'  then WT else 0 end) F_USE2_GAS"
			",sum(case when  mat_code in ('49093','49280','49092','49094') and EQU_NO like 'F%'  then CO2_WT else 0 end) F_USE_NY9"	  //消耗煤气
			",sum(case when  mat_code in ('49093','49280','49092','49094') and EQU_NO like 'F%'  then WT else 0 end) F_USE2_NY9"
			",sum(case when  mat_code in ('59103','59100','59101','59102') and EQU_NO like 'R%'  then CO2_WT else 0 end) R_USE_POWER"  //电
			",sum(case when  mat_code in ('59103','59100','59101','59102') and EQU_NO like 'R%'  then WT else 0 end) R_USE2_POWER"
			",sum(case when  mat_code ='59400' and EQU_NO like 'R%'  then CO2_WT else 0 end) R_USE_O2"	  //氧气
			",sum(case when  mat_code ='59400' and EQU_NO like 'R%'  then WT else 0 end) R_USE2_O2"
			",sum(case when  mat_code ='59402' and EQU_NO like 'R%'  then CO2_WT else 0 end) R_USE_AR"	 //氩气
			",sum(case when  mat_code ='59402' and EQU_NO like 'R%'  then WT else 0 end) R_USE2_AR"
			",sum(case when  mat_code ='59401' and EQU_NO like 'R%'  then CO2_WT else 0 end) R_USE_N"	  //氮气
			",sum(case when  mat_code ='59401' and EQU_NO like 'R%'  then WT else 0 end) R_USE2_N"
			",sum(case when  mat_code ='58001' and EQU_NO like 'R%'  then CO2_WT else 0 end) R_USE_H2O"	  //回收蒸汽
			",sum(case when  mat_code ='58001' and EQU_NO like 'R%'  then WT else 0 end) R_USE2_H2O"
			",sum(case when  mat_code ='48081' and EQU_NO like 'R%'  then CO2_WT else 0 end) R_USE_GAS"	  //回收转炉煤气
			",sum(case when  mat_code ='48081' and EQU_NO like 'R%'  then WT else 0 end) R_USE2_GAS"
			",sum(case when  mat_code in ('49093','49280','49092','49094') and EQU_NO like 'R%'  then CO2_WT else 0 end) R_USE_NY9"	  //消耗煤气
			",sum(case when  mat_code in ('49093','49280','49092','49094') and EQU_NO like 'R%'  then WT else 0 end) R_USE2_NY9"
			",sum(case when  mat_code in ('59103','59100','59101','59102') and EQU_NO like 'S%'  then CO2_WT else 0 end) S_USE_POWER"  //电
			",sum(case when  mat_code in ('59103','59100','59101','59102') and EQU_NO like 'S%'  then WT else 0 end) S_USE2_POWER"
			",sum(case when  mat_code ='59400' and EQU_NO like 'S%'  then CO2_WT else 0 end) S_USE_O2"	  //氧气
			",sum(case when  mat_code ='59400' and EQU_NO like 'S%'  then WT else 0 end) S_USE2_O2"
			",sum(case when  mat_code ='59402' and EQU_NO like 'S%'  then CO2_WT else 0 end) S_USE_AR"	 //氩气
			",sum(case when  mat_code ='59402' and EQU_NO like 'S%'  then WT else 0 end) S_USE2_AR"
			",sum(case when  mat_code ='59401' and EQU_NO like 'S%'  then CO2_WT else 0 end) S_USE_N"	  //氮气
			",sum(case when  mat_code ='59401' and EQU_NO like 'S%'  then WT else 0 end) S_USE2_N"
			",sum(case when  mat_code ='58001' and EQU_NO like 'S%'  then CO2_WT else 0 end) S_USE_H2O"	  //回收蒸汽
			",sum(case when  mat_code ='58001' and EQU_NO like 'S%'  then WT else 0 end) S_USE2_H2O"
			",sum(case when  mat_code ='48081' and EQU_NO like 'S%'  then CO2_WT else 0 end) S_USE_GAS"	  //回收转炉煤气
			",sum(case when  mat_code ='48081' and EQU_NO like 'S%'  then WT else 0 end) S_USE2_GAS"
			",sum(case when  mat_code in ('49093','49280','49092','49094') and EQU_NO like 'S%'  then CO2_WT else 0 end) S_USE_NY9"	  //消耗煤气
			",sum(case when  mat_code in ('49093','49280','49092','49094') and EQU_NO like 'S%'  then WT else 0 end) S_USE2_NY9"
			",sum(case when  mat_code in ('59103','59100','59101','59102') and EQU_NO like 'V%'  then CO2_WT else 0 end) V_USE_POWER"  //电
			",sum(case when  mat_code in ('59103','59100','59101','59102') and EQU_NO like 'V%'  then WT else 0 end) V_USE2_POWER"
			",sum(case when  mat_code ='59400' and EQU_NO like 'V%'  then CO2_WT else 0 end) V_USE_O2"	  //氧气
			",sum(case when  mat_code ='59400' and EQU_NO like 'V%'  then WT else 0 end) V_USE2_O2"
			",sum(case when  mat_code ='59402' and EQU_NO like 'V%'  then CO2_WT else 0 end) V_USE_AR"	 //氩气
			",sum(case when  mat_code ='59402' and EQU_NO like 'V%'  then WT else 0 end) V_USE2_AR"
			",sum(case when  mat_code ='59401' and EQU_NO like 'V%'  then CO2_WT else 0 end) V_USE_N"	  //氮气
			",sum(case when  mat_code ='59401' and EQU_NO like 'V%'  then WT else 0 end) V_USE2_N"
			",sum(case when  mat_code ='58001' and EQU_NO like 'V%'  then CO2_WT else 0 end) V_USE_H2O"	  //回收蒸汽
			",sum(case when  mat_code ='58001' and EQU_NO like 'V%'  then WT else 0 end) V_USE2_H2O"
			",sum(case when  mat_code ='48081' and EQU_NO like 'V%'  then CO2_WT else 0 end) V_USE_GAS"	  //回收转炉煤气
			",sum(case when  mat_code ='48081' and EQU_NO like 'V%'  then WT else 0 end) V_USE2_GAS"
			",sum(case when  mat_code in ('49093','49280','49092','49094') and EQU_NO like 'V%'  then CO2_WT else 0 end) V_USE_NY9"	  //消耗煤气
			",sum(case when  mat_code in ('49093','49280','49092','49094') and EQU_NO like 'V%'  then WT else 0 end) V_USE2_NY9"
			
			;

		sqlstr = " select t1.heat_no,t1.st_no,t1.prod_time,t1.PROD_WT,DEV_CODE_ROUTE"
			",t1.IRON_TEMP,t1.IRON_C,t1.IRON_SI,t1.IRON_MN,t1.IRON_P,t1.IRON_S"
			",t1.OUT_STEEL_TEMP,t1.FIN_C,t1.FIN_S,t1.FIN_P"
			",decode(PROD_WT,0,0,round(TYPE_4/prod_wt,6)) TYPE_4_UNIT"
			",t2.* "
			" from ttksm01 t1"
			" left join ("
			" select heat_no,sum(CO2_WT) CO2_WT" + sqlstr_sub +
			", sum(case when  mat_code in ( select mat_code from ttk0001 where type_code1 = '4') then CO2_WT else 0 end) TYPE_4"
			" from ttksm02"
			" where 1=1"
			" and PROD_TIME<=@end_time"
			" and PROD_TIME>=@begin_time"
			" group by heat_no"
			") t2 on t1.heat_no=t2.heat_no"
			" where 1=1"
			" and t1.PROD_TIME<=@end_time"
			" and t1.PROD_TIME>=@begin_time"
			;
		if (ttksm02["HEAT_NO"].ToString().Trim() != "")
		{
			sqlstr = sqlstr + " and t1.heat_no=@heat_no";
		}
		if (ttksm02["C_DIV"].ToString().Trim() != "")
		{
			sqlstr = sqlstr + " and C_DIV=@c_div";
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.Parameters.Set("heat_no", ttksm02["HEAT_NO"].ToString());
		cmd_inq.Parameters.Set("c_div", ttksm02["C_DIV"].ToString());
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