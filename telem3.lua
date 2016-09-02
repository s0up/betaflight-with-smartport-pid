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
  lcd.drawText(x,y,"s0uPiD")
  if (adj>1000 and adj<1100)     then o=1  lcd.drawText(x,y+z,"Pitch P",INVERS)
  elseif (adj>1100 and adj<1200) then o=2  lcd.drawText(x,y+z,"Pitch I",INVERS)
  elseif (adj>1200 and adj<1300) then o=3  lcd.drawText(x,y+z,"Pitch D",INVERS)
  elseif (adj>1300 and adj<1400) then o=4  lcd.drawText(x,y+z,"Roll P",INVERS)
  elseif (adj>1400 and adj<1480) then o=5  lcd.drawText(x,y+z,"Roll I",INVERS)
  elseif (adj>1520 and adj<1600) then o=6  lcd.drawText(x,y+z,"Roll D",INVERS)
  elseif (adj>1600 and adj<1700) then o=7  lcd.drawText(x,y+z,"Yaw P",INVERS)
  elseif (adj>1700 and adj<1800) then o=8  lcd.drawText(x,y+z,"Yaw I",INVERS)
  elseif (adj>1800 and adj<1900) then o=9  lcd.drawText(x,y+z,"Yaw D",INVERS)
  elseif (adj>1900 and adj<2000) then o=10  lcd.drawText(x,y+z,"RC Rate",INVERS)
  end
  if (o>0 and oo~=o) then playTone(1000,10,100,PLAY_NOW) end
  oo=o
  t2=getValue("Tmp1")
  if (t2>=0) then
    lcd.drawText(x,y+z*2,string.format("%.0f",t2))
  end
end

return { run=run_func,background=bg_func,init=init_func  }
