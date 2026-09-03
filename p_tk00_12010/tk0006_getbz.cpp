/// <summary>
/// 功能说明: 从动态成本那边获取标准物料消耗
/// </summary>
/// Copyright: Baosight Software LTD.co Copyright (c) 2010
/// Company:   上海宝信软件股份有限公司
/// Author:    
/// Version:   1.0
/// History:
///		

#include "stdafx.h"
// Service 入口
BM2F_ENTERACE(tk0006_getbz)
int f_tk00_bzco2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_tk0006_getbz(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	int doFlag = 0;
	int rowCount = 0;
	CString v_proc_div = "";
	int i = 0;
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString sqlstr = "";
	CDbCommand cmd_inq(conn);

	try
	{
		CDbCommand cmd(conn);

		CModel ttk0006("TTK0006");

		sqlstr = " delete from ttk0006"
			" where 1=1"
			" and st_no in ( select st_no from tqmtscb11a_dr where YEAR_MON in (select max(YEAR_MON) from tqmtscb11a_dr ))"
			" and TYPE_DESC = '主原料'"
			;
		Log::Trace("", "", "sqlstr={0}", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " insert into ttk0006 (rec_creator,rec_create_time,st_no,type_desc,mat_code,mat_name,wt)"
			" select @rec_creator, @rec_create_time,st_no,'主原料',mat_code,mat_name,sum(wt_unit)"
			" from tqmtscb11a_dr"
			" where YEAR_MON in (select max(YEAR_MON) from tqmtscb11a_dr )"
			" group by st_no,mat_code,mat_name"
			;
		Log::Trace("", "", "sqlstr={0}", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//更新标准碳排
		Log::Trace("", "", "11111111111");
		EIClass inBlock_yz, outBlock_yz;
		doFlag = f_tk00_bzco2(&inBlock_yz, &outBlock_yz, conn);
		if (doFlag != 0)
		{
			s.flag = -1;
			return -1;
		}

		Log::Trace("", "", "2222222");
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
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

	return doFlag;
}