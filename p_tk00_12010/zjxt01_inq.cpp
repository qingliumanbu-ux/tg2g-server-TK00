/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    3.0
Date:		2025-06-18
Description:专家系统宽表查询
**************************************************/
//框架用头文件
#include "stdafx.h"
#include "epex.h"  
// service入口
BM2F_ENTERACE(zjxt01_inq)
//-EP_SYSTEM_HEAD_END
int f_zjxt01_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int fetchRowCount = 0;

	CString begin_time("");
	CString end_time("");
	CString	heat_no = " ";
	CDecimal	tabFlag = 1;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CString  sqlstr("");
	CString  sqlstr_temp("");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_s(conn);

	try
	{
		
		begin_time = bcls_rec->Tables[0].Rows[0]["BEGIN_TIME"].ToString().SubstringNE(0, 8) + "000000";
		end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().SubstringNE(0, 8)+"235959";
		heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		tabFlag = bcls_rec->Tables[0].Rows[0]["TABFLAG"].ToDecimal();

		Log::Trace("", __FUNCTION__, "tabFlag = [{0}]", tabFlag);

		if (begin_time.GetLength() != 14 || end_time.GetLength() != 14 )
		{
			strcpy(s.msg, "开始结束时间不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (heat_no.Trim() != "")
		{
			sqlstr_temp +=  " AND t1.HEAT_NO = @heat_no";
		}
	
		if (end_time.Trim() != "")
		{
			sqlstr_temp +=  " AND t1.LADLE_CLOSE_TIME			<= @end_time";
		}
		if (begin_time.Trim() != "")
		{
			sqlstr_temp += " AND t1.LADLE_CLOSE_TIME			>= @begin_time";
		}

		if (tabFlag == 1 || tabFlag == 11
			|| tabFlag == 12
			|| tabFlag == 13)
		{

			sqlstr = "  SELECT * FROM ZJXT_KB T1 WHERE 1=1  "
				;

			sqlstr = sqlstr + sqlstr_temp;
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("begin_time", begin_time);
			cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}

		bcls_ret->Tables.Add();	//增加块
		if (tabFlag == 2)
		{

			sqlstr = " WITH MM1 AS (select L2_PROC_NO,DEV_CODE,DEVO_TIME,MAT_CODE,MAT_NAme,DEVO_WT,WEIGH_NO,LOT_NO "
               " from tmmsm2a_yl union "
               " select HEAT_NO, REMARK, DES_START, MATERIALID_ACT, '脱硫剂', ADDWGT_ACT, ' ', ' ' "
                " from tmmsmkr14) "
				" select  t1.heat_no,t2.L2_PROC_NO,t2.dev_code,t2.DEVO_TIME,t2.mat_code,t2.mat_name,t2.DEVO_WT,t2.WEIGH_NO,t2.LOT_NO"
				" from tmmsm31 t1"
				" left join mm1 t2 on t1.heat_no = t2.l2_proc_no"
				" where t1.st_no like '3%' AND T2.L2_PROC_NO IS NOT NULL  "
				;
			sqlstr = sqlstr + sqlstr_temp;
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("begin_time", begin_time);
			cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
			cmd_inq.Close();
		}


		bcls_ret->Tables.Add();	//增加块
		if (tabFlag == 3)
		{
			sqlstr = " select  *"
				" from tqmts24 t2"
				" where exists( select 1 from tmmsm31 t1 where t2.heat_no=t1.heat_no and  t1.st_no like '3%'"
				;
			sqlstr = sqlstr + sqlstr_temp + ")" + " order by  SAMPLE_TAKEN_TIME,ANALYSE_TIME";
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("begin_time", begin_time);
			cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[2]);
			cmd_inq.Close();
		}
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