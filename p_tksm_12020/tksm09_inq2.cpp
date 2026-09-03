/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    3.0
Date:		2023-09-22
Description:碳控排组成
**************************************************/
//框架用头文件
#include "stdafx.h"
// service入口
BM2F_ENTERACE(tksm09_inq2)
//-EP_SYSTEM_HEAD_END

int f_tksm09_inq2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int fetchRowCount = 0;

	CString begin_time("");
	CString end_time("");
	CString st_no("");
	CString backlog_ea("");

	CString		mat_type = " ";

	//CModel tcaais5("TCAAIS5");


	CString  sqlstr("");


	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_1(conn);

	try
	{
		begin_time = bcls_rec->Tables[0].Rows[0]["BEGIN_TIME"].ToString().SubstringNE(0, 8);
		end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().SubstringNE(0, 8);
		st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString(); 
		backlog_ea = bcls_rec->Tables[0].Rows[0]["AC_ROUTE"].ToString();

		Log::Trace("", __FUNCTION__, "backlog_ea = [{0}]st_no = [{1}]", backlog_ea, st_no);

		bcls_ret->Tables[0].Columns.Add(DT_STRING, "NAME");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "VALUE");
		bcls_ret->Tables[0].Rows.Add();
		bcls_ret->Tables[0].Rows[0]["NAME"] = "铁水";
		bcls_ret->Tables[0].Rows.Add();
		bcls_ret->Tables[0].Rows[1]["NAME"] = "废钢";
		bcls_ret->Tables[0].Rows.Add();
		bcls_ret->Tables[0].Rows[2]["NAME"] = "合金";
		bcls_ret->Tables[0].Rows.Add();
		bcls_ret->Tables[0].Rows[3]["NAME"] = "辅料";
		bcls_ret->Tables[0].Rows.Add();
		bcls_ret->Tables[0].Rows[4]["NAME"] = "电";
		bcls_ret->Tables[0].Rows.Add();
		bcls_ret->Tables[0].Rows[5]["NAME"] = "热力";


		//直排碳排量
		bcls_ret->Tables.Add();
		bcls_ret->Tables[1].Columns.Add(DT_STRING, "NAME");
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "VALUE");
		bcls_ret->Tables[1].Rows.Add();
		bcls_ret->Tables[1].Rows[0]["NAME"] = "铁水";
		bcls_ret->Tables[1].Rows.Add();
		bcls_ret->Tables[1].Rows[1]["NAME"] = "废钢";
		bcls_ret->Tables[1].Rows.Add();
		bcls_ret->Tables[1].Rows[2]["NAME"] = "合金";
		bcls_ret->Tables[1].Rows.Add();
		bcls_ret->Tables[1].Rows[3]["NAME"] = "辅料";
		bcls_ret->Tables[1].Rows.Add();
		bcls_ret->Tables[1].Rows[4]["NAME"] = "电";
		bcls_ret->Tables[1].Rows.Add();
		bcls_ret->Tables[1].Rows[5]["NAME"] = "热力";

		//上游碳排量
		bcls_ret->Tables.Add();
		bcls_ret->Tables[2].Columns.Add(DT_STRING, "NAME");
		bcls_ret->Tables[2].Columns.Add(DT_DECIMAL, "VALUE");
		bcls_ret->Tables[2].Rows.Add();
		bcls_ret->Tables[2].Rows[0]["NAME"] = "铁水";
		bcls_ret->Tables[2].Rows.Add();
		bcls_ret->Tables[2].Rows[1]["NAME"] = "废钢";
		bcls_ret->Tables[2].Rows.Add();
		bcls_ret->Tables[2].Rows[2]["NAME"] = "合金";
		bcls_ret->Tables[2].Rows.Add();
		bcls_ret->Tables[2].Rows[3]["NAME"] = "辅料";
		bcls_ret->Tables[2].Rows.Add();
		bcls_ret->Tables[2].Rows[4]["NAME"] = "电";
		bcls_ret->Tables[2].Rows.Add();
		bcls_ret->Tables[2].Rows[5]["NAME"] = "热力";


		sqlstr = " select sum(prod_wt) prod_wt"
			" from ttksm01"
			" where prod_time<=@end_time"
			" and prod_time>=@begin_time"
			" and st_no = @st_no"
			" and backlog_ea = @backlog_ea";
			; 
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.Parameters.Set("st_no", st_no);
		cmd_inq.Parameters.Set("backlog_ea", backlog_ea);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			sqlstr = " select  sum(case when mat_code ='TS0000' then  CO2_WT else 0 end) "
				",sum(case when mat_code in (select mat_code from ttk0001 where mat_code !='TS0000' and TYPE_CODE1 = '1' ) then CO2_WT else 0 end) "
				",sum(case when mat_code in (select mat_code from ttk0001 where TYPE_CODE1 = '2' ) then CO2_WT else 0 end) "
				",sum(case when mat_code in (select mat_code from ttk0001 where TYPE_CODE1 = '3' ) then CO2_WT else 0 end) "
				",sum(case when mat_code in (select mat_code from ttk0001 where MAT_CODE  in ('59100','59101','59102','59103')  ) then CO2_WT else 0 end) "
				",sum(case when mat_code in (select mat_code from ttk0001 where MAT_CODE  not in ('59100','59101','59102','59103')  and  TYPE_CODE1 = '4' ) then CO2_WT else 0 end) "
				",sum(CO2_WT) "
				" from ttksm02"
				" where 1=1"
				" and prod_time<=@end_time"
				" and prod_time>=@begin_time"
				" and st_no = @st_no"
				" and backlog_ea = @backlog_ea"
				;
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("begin_time", begin_time);
			cmd_inq_1.Parameters.Set("end_time", end_time);
			cmd_inq_1.Parameters.Set("st_no", st_no);
			cmd_inq_1.Parameters.Set("backlog_ea", backlog_ea);
			cmd_inq_1.ExecuteReader();
			if (cmd_inq_1.Read())
			{
				if (cmd_inq_1.GetDecimal(7) != 0)
				{
					bcls_ret->Tables[0].Rows[0]["VALUE"] = (100 * cmd_inq_1.GetDecimal(1) / cmd_inq_1.GetDecimal(7)).Round(3);
					bcls_ret->Tables[0].Rows[1]["VALUE"] = (100 * cmd_inq_1.GetDecimal(2) / cmd_inq_1.GetDecimal(7)).Round(3);
					bcls_ret->Tables[0].Rows[2]["VALUE"] = (100 * cmd_inq_1.GetDecimal(3) / cmd_inq_1.GetDecimal(7)).Round(3);
					bcls_ret->Tables[0].Rows[3]["VALUE"] = (100 * cmd_inq_1.GetDecimal(4) / cmd_inq_1.GetDecimal(7)).Round(3);
					bcls_ret->Tables[0].Rows[4]["VALUE"] = (100 * cmd_inq_1.GetDecimal(5) / cmd_inq_1.GetDecimal(7)).Round(3);
					bcls_ret->Tables[0].Rows[5]["VALUE"] = (100 * cmd_inq_1.GetDecimal(6) / cmd_inq_1.GetDecimal(7)).Round(3);

				}
			}
			cmd_inq_1.Close();

			//直排
			sqlstr = " select  sum(case when mat_code ='TS0000' then  CO2_WT1 else 0 end) "
				",sum(case when mat_code in (select mat_code from ttk0001 where mat_code !='TS0000' and TYPE_CODE1 = '1' ) then CO2_WT1 else 0 end) "
				",sum(case when mat_code in (select mat_code from ttk0001 where TYPE_CODE1 = '2' ) then CO2_WT1 else 0 end) "
				",sum(case when mat_code in (select mat_code from ttk0001 where TYPE_CODE1 = '3' ) then CO2_WT1 else 0 end) "
				",sum(case when mat_code in (select mat_code from ttk0001 where MAT_CODE  in ('59100','59101','59102','59103')  ) then CO2_WT1 else 0 end) "
				",sum(case when mat_code in (select mat_code from ttk0001 where MAT_CODE  not in ('59100','59101','59102','59103')  and  TYPE_CODE1 = '4' ) then CO2_WT1 else 0 end) "
				",sum(CO2_WT1) "
				" from ttksm02"
				" where 1=1"
				" and prod_time<=@end_time"
				" and prod_time>=@begin_time"
				" and st_no = @st_no"
				" and backlog_ea = @backlog_ea"
				;
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("begin_time", begin_time);
			cmd_inq_1.Parameters.Set("end_time", end_time);
			cmd_inq_1.Parameters.Set("st_no", st_no);
			cmd_inq_1.Parameters.Set("backlog_ea", backlog_ea);
			cmd_inq_1.ExecuteReader();
			if (cmd_inq_1.Read())
			{
				if (cmd_inq_1.GetDecimal(7) != 0)
				{
					bcls_ret->Tables[1].Rows[0]["VALUE"] = (100 * cmd_inq_1.GetDecimal(1) / cmd_inq_1.GetDecimal(7)).Round(3);
					bcls_ret->Tables[1].Rows[1]["VALUE"] = (100 * cmd_inq_1.GetDecimal(2) / cmd_inq_1.GetDecimal(7)).Round(3);
					bcls_ret->Tables[1].Rows[2]["VALUE"] = (100 * cmd_inq_1.GetDecimal(3) / cmd_inq_1.GetDecimal(7)).Round(3);
					bcls_ret->Tables[1].Rows[3]["VALUE"] = (100 * cmd_inq_1.GetDecimal(4) / cmd_inq_1.GetDecimal(7)).Round(3);
					bcls_ret->Tables[1].Rows[4]["VALUE"] = (100 * cmd_inq_1.GetDecimal(5) / cmd_inq_1.GetDecimal(7)).Round(3);
					bcls_ret->Tables[1].Rows[5]["VALUE"] = (100 * cmd_inq_1.GetDecimal(6) / cmd_inq_1.GetDecimal(7)).Round(3);

				}
				else
				{
					bcls_ret->Tables[1].Rows[0]["VALUE"] = 0;
					bcls_ret->Tables[1].Rows[1]["VALUE"] = 0;
					bcls_ret->Tables[1].Rows[2]["VALUE"] = 0;
					bcls_ret->Tables[1].Rows[3]["VALUE"] = 0;
					bcls_ret->Tables[1].Rows[4]["VALUE"] = 0;
					bcls_ret->Tables[1].Rows[5]["VALUE"] = 0;
				}
			}
			cmd_inq_1.Close();

			//直排
			sqlstr = " select  sum(case when mat_code ='TS0000' then  CO2_WT2 else 0 end) "
				",sum(case when mat_code in (select mat_code from ttk0001 where mat_code !='TS0000' and TYPE_CODE1 = '1' ) then CO2_WT2 else 0 end) "
				",sum(case when mat_code in (select mat_code from ttk0001 where TYPE_CODE1 = '2' ) then CO2_WT2 else 0 end) "
				",sum(case when mat_code in (select mat_code from ttk0001 where TYPE_CODE1 = '3' ) then CO2_WT2 else 0 end) "
				",sum(case when mat_code in (select mat_code from ttk0001 where MAT_CODE  in ('59100','59101','59102','59103')  ) then CO2_WT2 else 0 end) "
				",sum(case when mat_code in (select mat_code from ttk0001 where MAT_CODE  not in ('59100','59101','59102','59103')  and  TYPE_CODE1 = '4' ) then CO2_WT2 else 0 end) "
				",sum(CO2_WT1) "
				" from ttksm02"
				" where 1=1"
				" and prod_time<=@end_time"
				" and prod_time>=@begin_time"
				" and st_no = @st_no"
				" and backlog_ea = @backlog_ea"
				;
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("begin_time", begin_time);
			cmd_inq_1.Parameters.Set("end_time", end_time);
			cmd_inq_1.Parameters.Set("st_no", st_no);
			cmd_inq_1.Parameters.Set("backlog_ea", backlog_ea);
			cmd_inq_1.ExecuteReader();
			if (cmd_inq_1.Read())
			{
				if (cmd_inq_1.GetDecimal(7) != 0)
				{
					bcls_ret->Tables[2].Rows[0]["VALUE"] = (100 * cmd_inq_1.GetDecimal(1) / cmd_inq_1.GetDecimal(7)).Round(3);
					bcls_ret->Tables[2].Rows[1]["VALUE"] = (100 * cmd_inq_1.GetDecimal(2) / cmd_inq_1.GetDecimal(7)).Round(3);
					bcls_ret->Tables[2].Rows[2]["VALUE"] = (100 * cmd_inq_1.GetDecimal(3) / cmd_inq_1.GetDecimal(7)).Round(3);
					bcls_ret->Tables[2].Rows[3]["VALUE"] = (100 * cmd_inq_1.GetDecimal(4) / cmd_inq_1.GetDecimal(7)).Round(3);
					bcls_ret->Tables[2].Rows[4]["VALUE"] = (100 * cmd_inq_1.GetDecimal(5) / cmd_inq_1.GetDecimal(7)).Round(3);
					bcls_ret->Tables[2].Rows[5]["VALUE"] = (100 * cmd_inq_1.GetDecimal(6) / cmd_inq_1.GetDecimal(7)).Round(3);

				}
				else
				{
					bcls_ret->Tables[2].Rows[0]["VALUE"] = 0;
					bcls_ret->Tables[2].Rows[1]["VALUE"] = 0;
					bcls_ret->Tables[2].Rows[2]["VALUE"] = 0;
					bcls_ret->Tables[2].Rows[3]["VALUE"] = 0;
					bcls_ret->Tables[2].Rows[4]["VALUE"] = 0;
					bcls_ret->Tables[2].Rows[5]["VALUE"] = 0;
				}
			}
			cmd_inq_1.Close();

		}
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