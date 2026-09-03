/*=========================================================================
//程序名称:     f_caai_getprice
//隶属子系统:   CA
//产品名称:     BM2PES
//创建人员:     ZHOULI
//创建时间:     2012-11-26
//修改人员:     
//修改日期:     
//=========================================================================*/
//框架公用头文件，勿删
#include "stdafx.h"

//程序用头文件
BM2_FUNCTION_EXPORT
int f_tk00_getco2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int i = 0;
	int doFlag = 0;

	CString sqlstr = "";
	CModel ttk0004("TTK0004");

	// 创建电文处理对象
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_1(conn);

	try
	{
		ttk0004.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			
		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default: // 通用
			sqlstr = "select (case when t2.COE =0 then 1 else COE end)*t.CO2_COE as CO2_COE"
				",(case when t2.COE =0 then 1 else COE end)*t.CO2_COE1 as CO2_COE1"
				",(case when t2.COE =0 then 1 else COE end)*t.CO2_COE2 as CO2_COE2"
				",t.CO2_COE_UNIT,t.HOT_VAL,t.HOT_VAL_UNIT,t2.mat_name"
				" from TTK0004 t"
				" left join ttk0001 t2 on t.mat_code = t2.mat_code"
				" WHERE 1=1 "
				" AND t.mat_code = @mat_code "
				;			
			if (ttk0004["VALID_TIME"].ToString().Trim() != "")
			{
				sqlstr = sqlstr + "  and substr(t.VALID_TIME,1,8)<=@valid_time";
			}
				
			sqlstr = sqlstr + " ORDER BY  t.valid_time desc";
			
			break;
		}	
		//Log::Trace("", "", "sqlstr={0}", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("mat_code", ttk0004["MAT_CODE"].ToString());
		cmd_inq.Parameters.Set("valid_time", ttk0004["VALID_TIME"].ToString());
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.msg, (const char*)str, sizeof(s.msg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
