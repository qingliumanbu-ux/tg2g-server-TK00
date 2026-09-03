/*=========================================================================
//程序名称:     f_tk00_getsm
//隶属子系统:   TK
//产品名称:     BM2PES
//创建人员:     ZHOULI
//创建时间:     2012-11-26
//修改人员:   获取生产实绩信息  
//修改日期:     
//=========================================================================*/
//框架公用头文件，勿删
#include "stdafx.h"

//程序用头文件
BM2_FUNCTION_EXPORT
int f_tk00_getco2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_tk00_bzco2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int i = 0;
	int doFlag = 0;

	CString sqlstr = "";
	CString stat_date = "";
	CModel ttk0004("TTK0004");

	// 创建电文处理对象
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_1(conn);
	CDbCommand cmd_inq_s(conn);

	try
	{  
		//更新ttksm02的排放因子
		EIClass inBlock_yz, outBlock_yz;
		inBlock_yz.Tables[0].Columns.Add(DT_STRING, "MAT_CODE");
		inBlock_yz.Tables[0].Columns.Add(DT_STRING, "DATA_TYPE");
		inBlock_yz.Tables[0].Columns.Add(DT_STRING, "VALID_TIME");
		inBlock_yz.Tables[0].Rows.Add();
		sqlstr = " select  distinct mat_code from ttk0006"
			" where 1=1"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			inBlock_yz.Tables[0].Rows[0]["MAT_CODE"] = cmd_inq.GetString(1);
			doFlag = f_tk00_getco2(&inBlock_yz, &outBlock_yz, conn);
			if (doFlag != 0)
			{
				s.flag = -1;
				return -1;
			}
			if (outBlock_yz.Tables[0].Rows.get_Count() > 0)
			{
				Log::Trace("", "", "mat_code={0} ", cmd_inq.GetString(1));
				ttk0004.MergeFrom(outBlock_yz.Tables[0].Rows[0]);
				sqlstr = " update ttk0006 set CO2_COE= @co2_coe"
					",CO2_WT = round(wt*@co2_coe,6)"
					//" ,CO2_COE_UNIT = @co2_coe_unit"
					//",HOT_VAL=@hot_val"
					//",HOT_VAL_UNIT =@hot_val_unit"
					",mat_name = @mat_name"
					" where 1=1"
					" and mat_code = @mat_code"
					;
				cmd_inq_s.SetCommandText(sqlstr);
				cmd_inq_s.Parameters.Set("mat_code", cmd_inq.GetString(1));
				cmd_inq_s.Parameters.Set("mat_name", ttk0004["MAT_NAME"].ToString());
				cmd_inq_s.Parameters.Set("co2_coe", ttk0004["CO2_COE"].ToDecimal());
				cmd_inq_s.Parameters.Set("co2_coe_unit", ttk0004["CO2_COE_UNIT"].ToString());
				cmd_inq_s.Parameters.Set("hot_val", ttk0004["HOT_VAL"].ToDecimal());
				cmd_inq_s.Parameters.Set("hot_val_unit", ttk0004["HOT_VAL_UNIT"].ToString());
				cmd_inq_s.ExecuteNonQuery();
				cmd_inq_s.Close();
			}
		}
		cmd_inq.Close();
	
		  //不锈钢
		sqlstr = " update ttk0005 t1 set (CO2_WT,CO2_WT1,CO2_WT2) = (select sum(CO2_WT) CO2_WT,sum(CO2_WT1) CO2_WT1,sum(CO2_WT2) CO2_WT2 from (select sum(CO2_WT) CO2_WT,sum(CO2_WT1) CO2_WT1,sum(CO2_WT2) CO2_WT2 from ttk0006 t2 where t2.TYPE_DESC = '主原料'  and t2.st_no=t1.st_no"
		" union all select sum(CO2_WT) CO2_WT,sum(CO2_WT1) CO2_WT1,sum(CO2_WT2) CO2_WT2 from ttk0006 t2 where  t1.WHOLE_BACKLOG like '%'||t2.SUB_BACKLOG_CODE||'%' and t2.TYPE_DESC = '工序费'"
		"  union all select sum(CO2_WT) CO2_WT,sum(CO2_WT1) CO2_WT1,sum(CO2_WT2) CO2_WT2 from ttk0006 t2 where  t2.SUB_BACKLOG_CODE='1C' and TYPE_DESC = '工序费'))"
			" where type !='1'"
			" and EXISTS (select 1 from ttk0006 t2 where TYPE_DESC = '主原料'  and t2.st_no=t1.st_no)"
			" and substr(t1.st_no,1,1) in ('1','4')"
			;
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close(); 


		//碳钢
		sqlstr = " update ttk0005 t1 set (CO2_WT,CO2_WT1,CO2_WT2) =  (select sum(CO2_WT) CO2_WT,sum(CO2_WT1) CO2_WT1,sum(CO2_WT2) CO2_WT2 from (select sum(CO2_WT) CO2_WT,sum(CO2_WT1) CO2_WT1,sum(CO2_WT2) CO2_WT2 from ttk0006 t2 where TYPE_DESC = '主原料'  and t2.st_no=t1.st_no"
			" union all select sum(CO2_WT) CO2_WT,sum(CO2_WT1) CO2_WT1,sum(CO2_WT2) CO2_WT2 from ttk0006 t2 where  t1.WHOLE_BACKLOG like '%'||t2.SUB_BACKLOG_CODE||'%' and TYPE_DESC = '工序费'"
			"  union all select sum(CO2_WT) CO2_WT,sum(CO2_WT1) CO2_WT1,sum(CO2_WT2) CO2_WT2 from ttk0006 t2 where  t2.SUB_BACKLOG_CODE='2C' and TYPE_DESC = '工序费'))"
			" where type !='1'"
			" and EXISTS(select 1 from ttk0006 t2 where TYPE_DESC = '主原料'  and t2.st_no=t1.st_no)"
			" and substr(t1.st_no,1,1) in ('2','3','5')"
			;
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
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
