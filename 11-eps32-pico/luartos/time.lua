-- time.lua

time = {}

local dag = { "zondag", "maandag", "dinsdag", "woensdag", "donderdag", "vrijdag", "zaterdag" }
local maand = { "januari", "februari", "maart", "april", "mei", "juni", "juli", "augustus", "september", "oktober", "november", "december" }

local tzdata = {  -- 1616893200,  -- Lua begint te tellen op 1, dus de waarde voor nul staat niet in de table
            1635642000,
            1648342800,
            1667091600,
            1679792400,
            1698541200,
            1711846800,
            1729990800,
            1743296400,
            1761440400,
            1774746000,
            1792890000,
            1806195600,
            1824944400,
            1837645200,
            1856394000,
            1869094800,
            1887843600,
            1901149200,
            1919293200,
            1932598800,
            1950742800,
            1964048400,
            1982797200,
            1995498000,
            2014246800,
            2026947600,
            2045696400,
            2058397200,
            2077146000,
            2090451600,
            2108595600,
            2121901200,
            2140045200,
            2153350800,
            2172099600,
            2184800400,
            2203549200,
            2216250000,
            2234998800,
            2248304400,
            2266448400,
            2279754000,
            2297898000,
            2311203600,
            2329347600,
            2342653200,
            2361402000,
            2374102800,
            2392851600,
            2405552400,
            2424301200,
            2437606800,
            2455750800,
            2469056400,
            2487200400,
            2500506000,
            2519254800 }

function time.localtime()
  local t = os.time()
  local dst = false
  for i=1,#tzdata do
    if tzdata[i] > t then
      t = t + 3600 + 3600 * (i%2)
      dst = (i%2) == 1
      break
    end
  end
  local tb = os.date("!*t", t)
  tb.isdst = dst
  return tb
end

function time.longtime()
  local t = time.localtime()
  local dst = "CET"
  if t.isdst then
    dst = "CEST"
  end
  return string.format("%s %d %s %d, %2d:%02d:%02d %s", dag[t.wday], t.day, maand[t.month], t.year, t.hour, t.min, t.sec, dst)
end

function time.shorttime()
  local t = time.localtime()
  return string.format("%02d-%02d-%04d %02d:%02d:%02d", t.day, t.month, t.year, t.hour, t.min, t.sec)
end

return time
