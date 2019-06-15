import pymysql
from bs4 import BeautifulSoup
import urllib.request as pars

HOUR=""
POP=""
R06=""
REH=""
DATE=""
RAIN=''
rain = ''
TEMP = ''

# 경기도/화성시 RSS
japi = 'http://www.kma.go.kr/wid/queryDFSRSS.jsp?zone=4159025300'

response = pars.urlopen(japi)
weather = BeautifulSoup(response, "html.parser")

for element in weather.find_all('header'):
    DATE = element.tm.string[0:4] + "-" + element.tm.string[4:6] + "-" + element.tm.string[6:8]
    print(DATE)

for hr in weather.find_all('data', {"seq": "0"}):
    HOUR = hr.hour.string
    POP = hr.pop.string
    R06 = hr.r06.string
    REH = hr.reh.string
    rain = hr.r12.string
    TEMP = hr.temp.string
    print(HOUR) # 3시간후
    print(POP) # 강수확률
    print(R06)  #6시간 강수량
    print(REH) #습도
    print(rain) #비
    print(TEMP)

if int(POP) > 60:
    window = 1
elif int(REH) > 70:
    window = 1
else:
    window = 0

if rain == 'Rain':
    window = 1
    RAIN = 1
else:
    RAIN = 0

# DB 파싱 INIT
conn = pymysql.connect(host='13.124.166.146', user='root', db='rss')
curs = conn.cursor()


def select_db():
    num = curs.execute("SELECT hour FROM weather")
    print(num)

    tmps = curs.fetchall()
    aa = tmps[num-1]

    if aa[0] != HOUR:
        insert_db()


def insert_db():
    # SQL 쿼리 실행
    curs.execute("INSERT INTO weather values(%s, %s, %s, %s, %s, %s, %s, %s)", (DATE, HOUR, POP, R06, REH, window, RAIN, TEMP))
    conn.commit()


select_db()
conn.close()

