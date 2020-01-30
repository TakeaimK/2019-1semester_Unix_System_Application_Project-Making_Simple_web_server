# 2019-1semester_Unix_System_Application_Project-Making_Simple_web_server

### 2019-1학기 유닉스시스템응용프로젝트 간단한 웹 서버 제작

![유닉스캡처](https://user-images.githubusercontent.com/44010902/68129898-bed23780-ff5d-11e9-90fd-44690d1fa0d6.png)

---

IDE : VIM Editor  
Language : C  
Advisor : Prof. Heeseung Jo  
Team Member : Kim Seo-jin,Choi Se-jin

### How to use

1. Compile using Makefile (type "make" in shell)
2. Run if "simple_webserver" has been created
3. Keep server (ex. nohup)(ex2.create new putty client) and Login Other account
4. Unzip "http_load-test" and modify "urls" port number
5. Compile using Makefile (type "make" in shell)
6. Run test.sh

---

### 프로젝트 주제 및 조건

• 간단한 웹 서버 만들기  
• multiple client (browser)의 요청을 동시에 처리할 수 있어야 함  
• 웹서버 시작시 사용할 service directory 와 port 를 parameter 로 받는다.  
• 특별한 웹페이지인 합계출력 페이지(total.cgi)에 대한 처리를 수행한다.  
• 전송 기록을 log 파일로 저장한다.

### 예제

```
$ simple_webserver /example 19000
```

• 19000 포트를 열고 listening 상태에서 대기한다.  
• PC 의 web browser 로 http://iter1.jbnu.ac.kr:19000 요청시 service directory 인
/example 밑의 요청된 파일에 대한 전달을 수행한다.

- 요청하는 파일이 없을 경우 기본적으로 index.html (/example/index.html)파일을 읽
  어서 전송한다.  
  · index.html 파일도 없으면 "Not found" 텍스트를 전송한다.
- http://iter1.jbnu.ac.kr:19000/images/05_10.gif 요청시 /example/images/05_10.
  gif 을 전송 한다.  
  • 특별한 웹페이지인 total.cgi 를 요청할 경우 이제 대한 처리를 수행한다.
- 요청은 total.cgi?from=NNN&to=MMM 과 같은 방식으로 전달된다.
- NNN, MMM 는 숫자형태이며, 이러한 요청을 받으면 NNN ~ MMM 까지의 총합을 구해서 그 결과를 보여준다.  
  예) http://iter1.jbnu.ac.kr:19000/total.cgi?from=1&to=5 요청시 1~5 까지의 합인 15 를 화면에 보여준다.
- 웹서버는 매 전송시마다 log 로 기록한다.
- 로그는 log.txt 파일이름으로 저장하며, 기록 내용은 아래와 같은 포맷이다.
- 클라이언트 IP 전송한파일명 전송한크기 (3 가지 필드는 빈칸하나로 구분된다.)  
  · 예) 1.2.3.4 /images/05_10.gif 1391
- 로그는 매 전송시마다 기록되며, 한줄한줄이 섞이지 않아야 한다.
- 로그는 서버가 시작시 기존 로그를 지우고 새로운 파일로 시작한다.
- log.txt 는 서버가 시작한 디렉토리에 생성한다.

프로젝트 평가
• 정상적으로 동작하는지 확인  
• 첨부된 예제 홈페이지를 정상적으로 서비스 할 수 있는지 확인  
• 성능 향상을 위한 기능 추가 시 추가 점수 부여 (보고서에 어떠한 기능을 넣었는지 작성)

- 첨부된 예제 홈페이지와 total.cgi 만 서비스 할 수 있으면 됨
- 이를 고려하여 성능 향상 기능을 추가 할 것  
  • 포함된 http_load-test.tgz 을 이용하여 성능 테스트한 결과로 순위 결정 및 성적 반영
- 기준은 msecs/first-response: 0.137195 mean 값
- test.sh 을 이용하여 개발 시 테스트해 볼 것
- 평가시 urls 에 포함된 파라메터들은 변경될 수 있음
- make 명령으로 build되어야 함
