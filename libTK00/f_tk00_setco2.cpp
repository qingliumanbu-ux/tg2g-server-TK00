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
int f_tk00_setco2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int i = 0;
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CString sqlstr = "";
	CModel ttk0004("TTK0004");

	// 创建电文处理对象
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_1(conn);

	try
	{

		//获取最新的碳排信息
		sqlstr = " select nvl(t2.mat_Code, t1.mat_code) mat_code, nvl(t2.mat_name, ' ') mat_name, t1.mat_code as MAT_CODE_T, t1.CO2_COE, t1.CO2_COE1, t1.CO2_COE2, t1.VALID_TIME,nvl(t2.CO2_COE_UNIT,' ') AS CO2_COE_UNIT"
			" from(	 "
			" SELECT MAT_CODE, SUM(CO2_COE) CO2_COE, SUM(case when TYPE = '直排' then CO2_COE else 0 end) CO2_COE1, SUM(case when TYPE = '上游' then CO2_COE else 0 end) CO2_COE2, max(VALID_TIME) VALID_TIME "
			" from ttk0004c	  "
			" WHERE (MAT_CODE, VALID_TIME) IN(	"
			" SELECT MAT_CODE, MAX(VALID_TIME) FROM ttk0004c where  TYPE_DESC = '全球变暖潜力(GWP100):合计'  GROUP BY MAT_CODE) "
			" and TYPE_DESC = '全球变暖潜力(GWP100):合计'"
			" group by MAT_CODE	"
			" ) t1 left join ttk0001 t2 on t1.mat_Code = t2.mat_code_t "	
			" where nvl(t2.mat_code,' ')!=' '"
		;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			ttk0004.Reset();
			cmd_inq.Fetch(ttk0004);
			ttk0004["REC_CREATE_TIME"] = datetime;
			ttk0004["REC_CREATOR"] = s.userid;
			ttk0004["DATA_FROM"] = "易碳";
			if (ttk0004["CO2_COE_UNIT"].ToString().Trim() == "")
			{
				ttk0004["CO2_COE_UNIT"] = "kgCO2/kg";
			}			
			//判断是否最新的值是一样的，如果是则不插入，否则插入
			sqlstr = " select CO2_COE,CO2_COE1,CO2_COE2,VALID_TIME"
				" from ttk0004"
				" where VALID_TIME in (select max(VALID_TIME) from ttk0004 where VALID_TIME<=@valid_time and  MAT_CODE=@mat_code)"
				" and CO2_COE_UNIT =@co2_coe_unit"
				" and mat_code =@mat_code"
				;
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("co2_coe_unit", ttk0004["CO2_COE_UNIT"].ToString());
			cmd_inq_1.Parameters.Set("mat_code", ttk0004["MAT_CODE"].ToString());
			cmd_inq_1.Parameters.Set("valid_time", ttk0004["VALID_TIME"].ToString());
			cmd_inq_1.ExecuteReader();
			if (cmd_inq_1.Read())
			{
				if (ttk0004["CO2_COE"].ToDecimal() != cmd_inq_1.GetDecimal(1) || ttk0004["CO2_COE1"].ToDecimal() != cmd_inq_1.GetDecimal(2) || ttk0004["CO2_COE2"].ToDecimal() != cmd_inq_1.GetDecimal(3))
				{
					Log::Trace("", "", "mat_code={0},CO2_COE=[{1}],CO2_COE=[{2}],CO2_COE=[{3}],VALID_TIME=[{4}]", ttk0004["MAT_CODE"].ToString(), cmd_inq_1.GetDecimal(1), cmd_inq_1.GetDecimal(2), cmd_inq_1.GetDecimal(3), ttk0004["VALID_TIME"].ToString());
					if (ttk0004["VALID_TIME"].ToString() == cmd_inq_1.GetString(4))
					{
						ttk0004.Delete("MAT_CODE,VALID_TIME,CO2_COE_UNIT");
					}
					else
					{
						ttk0004.TrimOrBlank();
						ttk0004.Insert();
					}
				}
			}
			else
			{
				ttk0004.TrimOrBlank();
				ttk0004.Insert();
			}		
		cmd_inq_1.Close();

		}
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
