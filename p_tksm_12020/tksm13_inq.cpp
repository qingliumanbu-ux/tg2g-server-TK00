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
BM2F_ENTERACE(tksm13_inq)
//-EP_SYSTEM_HEAD_END

int f_tksm13_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int fetchRowCount = 0;

	CString begin_time("");
	CString end_time("");
	CString heat_no("");
	CString mat_no("");
	CDecimal mat_act_wt = 0;

	CString		mat_type = " ";

	//CModel tcaais5("TCAAIS5");


	CString  sqlstr("");


	CDbCommand cmd_inq(conn);

	try
	{

		bcls_ret->Tables[0].Columns.Add(DT_STRING, "炉号");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "材料号");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "出钢记号");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "牌号");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "产量");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "铁水温度");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "C成分");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "SI成分");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "P成分");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "S成分");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "出钢温度");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "冶炼时间");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "连浇炉数");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "铁钢比");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "铁水碳排总量");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "废钢碳排总量");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "合金碳排总量");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "辅料碳排总量");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "能介碳排总量");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "实绩碳排总量");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "铁水碳排强度");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "废钢碳排强度");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "合金碳排强度");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "辅料碳排强度");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "能介碳排强度");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "实绩碳排强度");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "预测碳排总量");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "预测碳排强度");

		bcls_ret->Tables[0].Rows.Add();

		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "脱硫碳排总量");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "脱硫碳排强度");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "中频炉碳排总量");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "中频炉碳排强度");		
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "转炉碳排总量");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "转炉碳排强度");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "电炉碳排总量");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "电炉碳排强度");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "A0D碳排总量");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "A0D碳排强度");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "精炼碳排总量");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "精炼碳排强度");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "连铸碳排总量");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "连铸碳排强度");	

		

		heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();  
		mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString();

		

		if (heat_no == "Load" || mat_no.Trim() == "Load" || (heat_no.Trim() == ""&& mat_no.Trim() == ""))
		{
			sqlstr = " select heat_no from ttksm01"
				" order by PROD_TIME desc"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				heat_no = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
		}

		if (heat_no.Trim() == ""&& mat_no.Trim() != "")
		{
			sqlstr = " select heat_no,mat_act_wt from tmmsm01 where mat_no=@mat_no"
				" union "
				" select heat_no,mat_act_wt from hmmsm01 where mat_no=@mat_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("mat_no", mat_no);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				heat_no = cmd_inq.GetString(1);
				mat_act_wt = cmd_inq.GetDecimal(2);
			}
			cmd_inq.Close();
		}
		else
		{
			sqlstr = " select min(mat_no),max(mat_no),sum(mat_act_wt) from ("
				" select mat_no,mat_act_wt from tmmsm01 where heat_no=@heat_no"
				" union "
				" select mat_no,mat_act_wt from hmmsm01 where heat_no=@heat_no"
				" )"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				if (cmd_inq.GetString(1) != cmd_inq.GetString(2))
				{
					mat_no = cmd_inq.GetString(1) + "--" + cmd_inq.GetString(2);
				}
				else
				{
					mat_no = cmd_inq.GetString(1);
				}
				mat_act_wt = cmd_inq.GetDecimal(3);
			}
			cmd_inq.Close();
		}
		

		bcls_ret->Tables[0].Rows[0]["炉号"] = heat_no;
		bcls_ret->Tables[0].Rows[0]["材料号"] = mat_no;
		

		sqlstr = " select ST_NO as 出钢记号,SG_SIGN as 牌号,IRON_TEMP as 铁水温度,IRON_C as C成分,IRON_SI as SI成分,IRON_P as P成分,IRON_S as S成分,OUT_STEEL_TEMP as 出钢温度,BOF_TIME+AOD_TIME+EAF_TIME+LF_TIME+RH_TIME+VD_TIME+VOD_TIME+LTS_TIME+CC_TIME as 冶炼时间"
			",decode(PROD_WT,0,0,round(nvl((select wt from ttksm02 t2 where t2.mat_code = 'TS0000' and t1.heat_no=t2.heat_no),0)/PROD_WT,3)) as 铁钢比"
			" ,(select sum(co2_wt) from ttksm02 t2 where t2.mat_code = 'TS0000' and t1.heat_no=t2.heat_no)  铁水碳排总量"
			" ,(select sum(co2_wt) from ttksm02 t2 where t2.mat_code in (select mat_code from ttk0001 where type_code1='1') and t1.heat_no=t2.heat_no)  废钢碳排总量"
			" ,(select sum(co2_wt) from ttksm02 t2 where t2.mat_code in (select mat_code from ttk0001 where type_code1='2') and t1.heat_no=t2.heat_no)  合金碳排总量"
			" ,(select sum(co2_wt) from ttksm02 t2 where t2.mat_code in (select mat_code from ttk0001 where type_code1='3') and t1.heat_no=t2.heat_no)  辅料碳排总量"
			" ,(select sum(co2_wt) from ttksm02 t2 where t2.mat_code in (select mat_code from ttk0001 where type_code1='4') and t1.heat_no=t2.heat_no)  能介碳排总量"
			" ,(select sum(co2_wt) from ttksm02 t2 where  t1.heat_no=t2.heat_no)  实绩碳排总量"
			",PROD_WT" 	
			" ,nvl((select sum(co2_wt) from ttksm02 t2 where (EQU_NO like 'D%' OR COST_CENTER IN ('EGBA')) and t1.heat_no=t2.heat_no),0)  脱硫碳排总量"
			" ,nvl((select sum(co2_wt) from ttksm02 t2 where (EQU_NO like 'Z%' OR COST_CENTER IN ('EGBG'))  and t1.heat_no=t2.heat_no),0)  中频炉碳排总量"
			" ,nvl((select sum(co2_wt) from ttksm02 t2 where (EQU_NO like 'B%' OR COST_CENTER IN ('EGUA','EGBB')) and t1.heat_no=t2.heat_no),0)  转炉碳排总量"
			" ,nvl((select sum(co2_wt) from ttksm02 t2 where (EQU_NO like 'E%' OR COST_CENTER IN ('EGBF')) and t1.heat_no=t2.heat_no),0)  电炉碳排总量"
			" ,nvl((select sum(co2_wt) from ttksm02 t2 where (EQU_NO like 'A%'  OR COST_CENTER IN ('EG3A','EGBH')) and t1.heat_no=t2.heat_no),0)  AOD碳排总量"
			" ,nvl((select sum(co2_wt) from ttksm02 t2 where (substr(EQU_NO,1,1) in ('R' ,'V','F')  OR COST_CENTER IN ('EG2A','EGBC','EGBD','EGBI')) and t1.heat_no=t2.heat_no),0)  精炼碳排总量"
			" ,nvl((select sum(co2_wt) from ttksm02 t2 where (EQU_NO like 'C%'  OR COST_CENTER IN ('EGBK','EGBL','EGBJ')) and t1.heat_no=t2.heat_no),0)  连铸碳排总量"
			" from ttksm01 t1"
			" where heat_no=@heat_no"
			; 
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", heat_no);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			bcls_ret->Tables[0].Rows[0]["出钢记号"] = cmd_inq.GetString(1);
			bcls_ret->Tables[0].Rows[0]["牌号"] = cmd_inq.GetString(2);
			bcls_ret->Tables[0].Rows[0]["铁水温度"] = cmd_inq.GetDecimal(3);
			bcls_ret->Tables[0].Rows[0]["C成分"] = cmd_inq.GetDecimal(4);
			bcls_ret->Tables[0].Rows[0]["SI成分"] = cmd_inq.GetDecimal(5);
			bcls_ret->Tables[0].Rows[0]["P成分"] = cmd_inq.GetDecimal(6);
			bcls_ret->Tables[0].Rows[0]["S成分"] = cmd_inq.GetDecimal(7);
			bcls_ret->Tables[0].Rows[0]["出钢温度"] = cmd_inq.GetDecimal(8);
			bcls_ret->Tables[0].Rows[0]["冶炼时间"] = cmd_inq.GetDecimal(9);
			bcls_ret->Tables[0].Rows[0]["铁钢比"] = cmd_inq.GetString(10)+"%";
			bcls_ret->Tables[0].Rows[0]["铁水碳排总量"] = cmd_inq.GetDecimal(11);
			bcls_ret->Tables[0].Rows[0]["废钢碳排总量"] = cmd_inq.GetDecimal(12);
			bcls_ret->Tables[0].Rows[0]["合金碳排总量"] = cmd_inq.GetDecimal(13);
			bcls_ret->Tables[0].Rows[0]["辅料碳排总量"] = cmd_inq.GetDecimal(14);
			bcls_ret->Tables[0].Rows[0]["能介碳排总量"] = cmd_inq.GetDecimal(15);
			bcls_ret->Tables[0].Rows[0]["实绩碳排总量"] = cmd_inq.GetDecimal(16);
			bcls_ret->Tables[0].Rows[0]["产量"] = cmd_inq.GetDecimal(17);
			bcls_ret->Tables[0].Rows[0]["脱硫碳排总量"] = cmd_inq.GetDecimal(18);
			bcls_ret->Tables[0].Rows[0]["中频炉碳排总量"] = cmd_inq.GetDecimal(19);
			bcls_ret->Tables[0].Rows[0]["转炉碳排总量"] = cmd_inq.GetDecimal(20);
			bcls_ret->Tables[0].Rows[0]["电炉碳排总量"] = cmd_inq.GetDecimal(21);
			bcls_ret->Tables[0].Rows[0]["A0D碳排总量"] = cmd_inq.GetDecimal(22);
			bcls_ret->Tables[0].Rows[0]["精炼碳排总量"] = cmd_inq.GetDecimal(23);
			bcls_ret->Tables[0].Rows[0]["连铸碳排总量"] = cmd_inq.GetDecimal(24);

			if (bcls_ret->Tables[0].Rows[0]["产量"].ToDecimal() != 0)
			{
				bcls_ret->Tables[0].Rows[0]["铁水碳排强度"] = (bcls_ret->Tables[0].Rows[0]["铁水碳排总量"].ToDecimal() / (bcls_ret->Tables[0].Rows[0]["产量"].ToDecimal())).Round(4);
				bcls_ret->Tables[0].Rows[0]["废钢碳排强度"] = (bcls_ret->Tables[0].Rows[0]["废钢碳排总量"].ToDecimal() / (bcls_ret->Tables[0].Rows[0]["产量"].ToDecimal())).Round(4);
				bcls_ret->Tables[0].Rows[0]["合金碳排强度"] = (bcls_ret->Tables[0].Rows[0]["合金碳排总量"].ToDecimal() / (bcls_ret->Tables[0].Rows[0]["产量"].ToDecimal())).Round(4);
				bcls_ret->Tables[0].Rows[0]["辅料碳排强度"] = (bcls_ret->Tables[0].Rows[0]["辅料碳排总量"].ToDecimal() / (bcls_ret->Tables[0].Rows[0]["产量"].ToDecimal())).Round(4);
				bcls_ret->Tables[0].Rows[0]["能介碳排强度"] = (bcls_ret->Tables[0].Rows[0]["能介碳排总量"].ToDecimal() / (bcls_ret->Tables[0].Rows[0]["产量"].ToDecimal())).Round(4);
				bcls_ret->Tables[0].Rows[0]["实绩碳排强度"] = (bcls_ret->Tables[0].Rows[0]["实绩碳排总量"].ToDecimal() / (bcls_ret->Tables[0].Rows[0]["产量"].ToDecimal())).Round(4);

				bcls_ret->Tables[0].Rows[0]["脱硫碳排强度"] = (bcls_ret->Tables[0].Rows[0]["脱硫碳排总量"].ToDecimal() / (bcls_ret->Tables[0].Rows[0]["产量"].ToDecimal())).Round(4);
				bcls_ret->Tables[0].Rows[0]["中频炉碳排强度"] = (bcls_ret->Tables[0].Rows[0]["中频炉碳排总量"].ToDecimal() / (bcls_ret->Tables[0].Rows[0]["产量"].ToDecimal())).Round(4);
				bcls_ret->Tables[0].Rows[0]["转炉碳排强度"] = (bcls_ret->Tables[0].Rows[0]["转炉碳排总量"].ToDecimal() / (bcls_ret->Tables[0].Rows[0]["产量"].ToDecimal())).Round(4);
				bcls_ret->Tables[0].Rows[0]["电炉碳排强度"] = (bcls_ret->Tables[0].Rows[0]["电炉碳排总量"].ToDecimal() / (bcls_ret->Tables[0].Rows[0]["产量"].ToDecimal())).Round(4);
				bcls_ret->Tables[0].Rows[0]["A0D碳排强度"] = (bcls_ret->Tables[0].Rows[0]["A0D碳排总量"].ToDecimal() / (bcls_ret->Tables[0].Rows[0]["产量"].ToDecimal())).Round(4);
				bcls_ret->Tables[0].Rows[0]["精炼碳排强度"] = (bcls_ret->Tables[0].Rows[0]["精炼碳排总量"].ToDecimal() / (bcls_ret->Tables[0].Rows[0]["产量"].ToDecimal())).Round(4);
				bcls_ret->Tables[0].Rows[0]["连铸碳排强度"] = (bcls_ret->Tables[0].Rows[0]["连铸碳排总量"].ToDecimal() / (bcls_ret->Tables[0].Rows[0]["产量"].ToDecimal())).Round(4);
			}
		}
		cmd_inq.Close();

		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "铁水系数");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "废钢系数");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "合金系数");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "辅料系数");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "能介系数");
		if (bcls_ret->Tables[0].Rows[0]["实绩碳排总量"].ToDecimal()!=0)
		{
			bcls_ret->Tables[0].Rows[0]["铁水系数"] = (bcls_ret->Tables[0].Rows[0]["铁水碳排总量"].ToDecimal() / bcls_ret->Tables[0].Rows[0]["实绩碳排总量"].ToDecimal()).Round(6);
			bcls_ret->Tables[0].Rows[0]["废钢系数"] = (bcls_ret->Tables[0].Rows[0]["废钢碳排总量"].ToDecimal() / bcls_ret->Tables[0].Rows[0]["实绩碳排总量"].ToDecimal()).Round(6);
			bcls_ret->Tables[0].Rows[0]["合金系数"] = (bcls_ret->Tables[0].Rows[0]["合金碳排总量"].ToDecimal() / bcls_ret->Tables[0].Rows[0]["实绩碳排总量"].ToDecimal()).Round(6);
			bcls_ret->Tables[0].Rows[0]["辅料系数"] = (bcls_ret->Tables[0].Rows[0]["辅料碳排总量"].ToDecimal() / bcls_ret->Tables[0].Rows[0]["实绩碳排总量"].ToDecimal()).Round(6);
			bcls_ret->Tables[0].Rows[0]["能介系数"] = (bcls_ret->Tables[0].Rows[0]["实绩碳排总量"].ToDecimal() / bcls_ret->Tables[0].Rows[0]["实绩碳排总量"].ToDecimal()).Round(6);

		}

		//获取冶炼时间
		sqlstr = " select sum(DURATION_TIME)"
			" from tmmsmgy06"
			" where heat_no = @heat_no"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", heat_no);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			bcls_ret->Tables[0].Rows[0]["冶炼时间"] = cmd_inq.GetDecimal(1);
		}
		cmd_inq.Close();

		//获取冶炼时间
		sqlstr = " select count(1)"
			" from tmmsmgy05"
			" where 1=1"
			" and (cast_div_no,TD_NO_1,CAST_DIV_NO_1) in   "
			" (select cast_div_no, TD_NO_1, CAST_DIV_NO_1 from tmmsmgy05 where heat_no = @heat_no)"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", heat_no);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			bcls_ret->Tables[0].Rows[0]["连浇炉数"] = cmd_inq.GetDecimal(1);
		}
		cmd_inq.Close();   

		bcls_ret->Tables.Add();
		bcls_ret->Tables[1].Columns.Add(DT_STRING, "路径");
		bcls_ret->Tables[1].Columns.Add(DT_STRING, "默认路径");
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "碳排总量");
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "碳排强度");  		
		sqlstr = " select st_no as 出钢记号"
			" ,nvl((SELECT CODE_DESC_1_CONTENT  FROM TEP0002 t2 WHERE CODE = t1.WHOLE_BACKLOG AND CODE_CLASS = 'TK04'),WHOLE_BACKLOG) 路径"
			" ,DEFAULT_FLAG 默认路径"
			",CO2_WT 碳排强度"
			", WHOLE_BACKLOG "
			" from ttk0005 t1"
			" where st_no=@st_no"
			" order by DEFAULT_FLAG desc"
			;	
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("st_no", bcls_ret->Tables[0].Rows[0]["出钢记号"].ToString());
		cmd_inq.ExecuteReader();
		int i = 0;
		while (cmd_inq.Read())
		{ 
			
			bcls_ret->Tables[1].Rows.Add();
			bcls_ret->Tables[1].Rows[i]["路径"] = cmd_inq.GetString(2);		
			bcls_ret->Tables[1].Rows[i]["默认路径"] = cmd_inq.GetString(3);
			bcls_ret->Tables[1].Rows[i]["碳排强度"] = (cmd_inq.GetDecimal(4)).Round(4);
			bcls_ret->Tables[1].Rows[i]["碳排总量"] = (cmd_inq.GetDecimal(4)*bcls_ret->Tables[0].Rows[0]["产量"].ToDecimal()).Round(3);

			if (cmd_inq.GetString(3) == "1")
			{
				bcls_ret->Tables[0].Rows[0]["预测碳排强度"] = (cmd_inq.GetDecimal(4)).Round(4);
				bcls_ret->Tables[0].Rows[0]["预测碳排总量"] = (cmd_inq.GetDecimal(4)*bcls_ret->Tables[0].Rows[0]["产量"].ToDecimal()).Round(3);
			}
			i++;
		} 
		cmd_inq.Close(); 

		Log::Info("", __FUNCTION__, "st_no =[{0}],路径 =[{1}]", bcls_ret->Tables[0].Rows[0]["铁钢比"], bcls_ret->Tables[1].Rows[0]["路径"]);


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