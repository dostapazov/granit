CREATE PROCEDURE SPCALCTEST (
  FORMULA VARCHAR(128)
) RETURNS (
  CALCRESULT NUMERIC(10, 10)
) AS                                    
DECLARE VARIABLE CalcHandle integer;
DECLARE VARIABLE CalcError  integer;
DECLARE VARIABLE CalcString char(128);

BEGIN

 CalcHandle = IB_XP_CALC_CREATE();
 CalcString = '0'; 
  if(CalcHandle <>0) then
  begin
   CalcResult = IB_XP_CALC_FORMULA(:CalcHandle,:Formula);  
   CalcError  = IB_XP_CALC_ERROR(:CalcHandle);

   if(CalcError <> 0) then
   begin
   CalcResult = NULL;
   end
   CalcError = IB_XP_CALC_FREE(:CalcHandle);
  end
 
END