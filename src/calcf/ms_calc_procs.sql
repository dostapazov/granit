/* Microsoft SQL Server - Scripting			*/
/* Server: (LOCAL)					*/
/* Database: master					*/
/* Creation Date 25.01.01 17:20:40 			*/

if exists (select * from sysobjects where id = object_id(N'[dbo].[msxp_CalcGetError]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[msxp_CalcGetError]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[msxp_AddCalculatorVariable]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[msxp_AddCalculatorVariable]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[msxp_CalcClearVariables]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[msxp_CalcClearVariables]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[msxp_AllocCalculator]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[msxp_AllocCalculator]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[msxp_CalcFormula]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[msxp_CalcFormula]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[msxp_FreeCalculator]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[msxp_FreeCalculator]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[sp_CalcAddVariable]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[sp_CalcAddVariable]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[sp_CalcAddVariableAsStr]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[sp_CalcAddVariableAsStr]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[sp_CalcClearVariables]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[sp_CalcClearVariables]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[sp_CalcCreate]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[sp_CalcCreate]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[sp_CalcFormula]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[sp_CalcFormula]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[sp_CalcFormulaImmediate]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[sp_CalcFormulaImmediate]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[sp_CalcFree]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[sp_CalcFree]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[sp_CalcGetError]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[sp_CalcGetError]
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  OFF 
GO

exec sp_addextendedproc N'msxp_CalcGetError', N'D:\USER\GRANIT\SRC\CALCF\CALCF.DLL'
GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  OFF 
GO

exec sp_addextendedproc N'msxp_AddCalculatorVariable', N'D:\USER\GRANIT\SRC\CALCF\CALCF.DLL'
GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  OFF 
GO

exec sp_addextendedproc N'msxp_CalcClearVariables', N'D:\USER\GRANIT\SRC\CALCF\CALCF.DLL'
GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  OFF 
GO

exec sp_addextendedproc N'msxp_AllocCalculator', N'D:\USER\GRANIT\SRC\CALCF\CALCF.DLL'
GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  OFF 
GO

exec sp_addextendedproc N'msxp_CalcFormula', N'D:\USER\GRANIT\SRC\CALCF\CALCF.DLL'
GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  OFF 
GO

exec sp_addextendedproc N'msxp_FreeCalculator', N'D:\USER\GRANIT\SRC\CALCF\CALCF.DLL'
GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

CREATE PROCEDURE sp_CalcAddVariable  @@CalcHandle integer, @@X float AS 
begin
 if(@@CalcHandle <> 0)
  begin
  declare @@X_STR varchar(256);
  Set @@X_STR = convert(varchar(256),@@X);
  execute msxp_AddCalculatorVariable @@CalcHandle, @@X_STR;
 end
end

GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

CREATE PROCEDURE sp_CalcAddVariable @@CalcHandle integer, @@X varchar(128) AS 
begin
 if(@@CalcHandle <> 0)
  execute msxp_AddCalculatorVariable @@CalcHandle, @@X;
end

GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

CREATE PROCEDURE sp_CalcClearVariables @@CalcHandle integer AS
begin
exec msxp_CalcClearVariables @@CalcHandle;
end

GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

CREATE PROCEDURE sp_CalcCreate @@CalcHandle integer OUTPUT AS 
 begin
  exec msxp_AllocCalculator @@CalcHandle OUTPUT;
 end



GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

CREATE PROCEDURE sp_CalcFormula @@CalcHandle integer, @@Formula varchar(1024), @@Result Float OUTPUT AS
declare @@CalcResult varchar(1024);
declare @@CalcError    integer;
begin
 if(@@CalcHandle <>0) 
 begin
  exec @@CalcError = msxp_CalcFormula  @@CalcResult OUTPUT, @@CalcHandle, @@Formula ;
  if(@@CalcError <> 0) 
 begin
  Set @@Result = convert(float,@@CalcResult);
 end
   else
   Set @@Result = NULL;
 end
end










GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

CREATE PROCEDURE sp_CalcFormulaImmediate @@Formula Varchar(1024), @@Result real OUTPUT AS
declare @@CalcHandle integer;
declare @@ErrorCode   integer;
begin
   exec sp_CalcCreate @@CalcHandle OUTPUT;
   select @@CalcHandle;
   if(@@CalcHandle <> 0)
    begin
      exec sp_CalcFormula @@CalcHandle,@@Formula,@@Result OUTPUT;
      
      exec sp_CalcFree @@CalcHandle;
    end
     else
     Set @@Result = NULL;
     /*insert into aa2 values(1,@@Formula,@@Result);*/
     select @@Result;
end





GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

CREATE PROCEDURE sp_CalcFree @@CalcHandle integer  AS
begin
execute msxp_FreeCalculator  @@CalcHandle;
end


GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

CREATE PROCEDURE sp_CalcGetError @@CalcHandle integer, @@ErrorCode integer OUTPUT AS
begin
 exec msxp_CalcGetError @@ErrorCode,@@CalcHandle;
end
GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

