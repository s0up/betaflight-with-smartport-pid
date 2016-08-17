local function init_func()
end

local function bg_func()
end

local function run_func(event)
  bg_func() 
  lcd.clear()
  local x=5
  local y=5
  local o=0
  local adj=((getValue("s2")+1024)/2048*1024)+987
  local z=10
  lcd.drawText(x,y,"Adjust Mode")
  if (adj>1355 and adj<1395)     then o=1  lcd.drawText(x,y+z,"Pit/Rol P",INVERS)
  elseif (adj>1280 and adj<1320) then o=3  lcd.drawText(x,y+z,"Pit/Rol I",INVERS)
  elseif (adj>1205 and adj<1245) then o=5  lcd.drawText(x,y+z,"Pit/Rol D",INVERS)
  elseif (adj>1130 and adj<1170) then o=7  lcd.drawText(x,y+z,"Yaw P",INVERS)
  elseif (adj>1055 and adj<1095) then o=9  lcd.drawText(x,y+z,"Yaw I",INVERS)
  elseif (adj> 900 and adj<1020) then o=11 lcd.drawText(x,y+z,"Yaw D",INVERS)
  elseif (adj>1605 and adj<1645) then o=2  lcd.drawText(x,y+z,"Pit/Rol Rate",INVERS)
  elseif (adj>1680 and adj<1720) then o=4  lcd.drawText(x,y+z,"Yaw Rate",INVERS)
  elseif (adj>1755 and adj<1795) then o=6  lcd.drawText(x,y+z,"RC Rate",INVERS)
  elseif (adj>1830 and adj<1870) then o=8  lcd.drawText(x,y+z,"RC Expo",INVERS)
  elseif (adj>1905 and adj<2100) then o=10 lcd.drawText(x,y+z,"Thr Expo",INVERS)
  end
  if (o>0 and oo~=o) then playTone(1000,10,100,PLAY_NOW) end
  oo=o
  t2=getValue("Tmp2")
  if (t2>=0) then
    qa=t2%1000
    qb=(t2-qa)/1000
    if (o==1)      then lcd.drawText(x,y+z*2,string.format("%.0f/%.0f",qa,qb))
    elseif (o==3)  then lcd.drawText(x,y+z*2,string.format("%.0f/%.0f",qa,qb))
    elseif (o==5)  then lcd.drawText(x,y+z*2,string.format("%.0f/%.0f",qa,qb))
    elseif (o==7)  then lcd.drawText(x,y+z*2,string.format("%.0f",t2))
    elseif (o==9)  then lcd.drawText(x,y+z*2,string.format("%.0f",t2))
    elseif (o==11) then lcd.drawText(x,y+z*2,string.format("%.0f",t2))
    elseif (o==2)  then lcd.drawText(x,y+z*2,string.format("%.2f/%.2f",qa/100,qb/100))
    elseif (o==4)  then lcd.drawText(x,y+z*2,string.format("%.2f",t2/100))
    elseif (o==6)  then lcd.drawText(x,y+z*2,string.format("%.2f",t2/100))
    elseif (o==8)  then lcd.drawText(x,y+z*2,string.format("%.2f",t2/100))
    elseif (o==10) then lcd.drawText(x,y+z*2,string.format("%.2f",t2/100))
    end
  end
end

return { run=run_func,background=bg_func,init=init_func  }
