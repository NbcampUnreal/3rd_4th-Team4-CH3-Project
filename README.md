# 『 PPP : Purge Protocol Project 』

![Project Thumbnail](PPP_Thumbnail.png)

---

## 📖 프로젝트 소개

**📆 제작 기간: 2025.07.28 ~ 2025.08.18**

### 🌌 세계관  
세상은 이미 끝났다.
도시는 불타고, 하늘은 탁해졌으며, 인류는 자신들이 만든 전염과 광기에 잠식당했다.
남은 인간들은 더 이상 이성을 가진 존재가 아니라, 오직 파괴 본능만 남은 괴물일 뿐이다.

그들을 정리하기 위해, 마지막으로 남은 **청소 로봇(Protocol Cleaner)**이 가동된다.
로봇은 감정도 망설임도 없다.
프로토콜은 단 하나.

> “인간을 제거하고, 세상을 정화하라.”

<br>

### 🎮 Gameplay

- 장르 : 서바이벌 TPS / FPS
- 전투 : 인간을 처치하면 Score가 누적된다.
- 미션 : 목표를 달성하면 보상 무기가 지급된다.
- 핵심 목표 : 점수를 쌓고, 무기를 확장하며, 끝없이 이어지는 전투 속에서 생존하라.

<br>

## 🎯 프로젝트 목표

- **아포칼립스 생존 슈팅 루프 완성**
  처치 → 스코어 획득 → 미션(처치 수) 완료 → 무기 보상 → Total Score
- **TPS/FPS 시점 지원**
  로봇 뒤(3인칭) 기본, 필요 시 1인칭 전환
- **델리게이트 기반 아키텍처**
  낮은 결합도: 캐릭터/무기 ↔ UI/게임모드 분리, 교체·테스트 용이
- **튜토리얼 중심의 첫 번째 레벨**
  이동/사격/스코어/미션 규칙을 자연스럽게 안내
- **아웃게임 UI**
  비디오 배경 메인 메뉴, 정지 메뉴(Pause), 게임오버 메뉴 구현

<br>

## 🧩 주요 기능

- **전투**
    - **게임모드 / GameMode**
        - 히트마커, 크로스헤어, 재장전 자체 로직은 캐릭터/무기에서 처리
    - **UI/HUD**
        - 히트마커 표시
        - 크로스헤어 표시
        - 재장전 상태 표시 (탄약 수, 재장전 애니메이션 동기화)
- **스코어링**
    - **게임모드 / GameMode**
        - 적 처치 시 점수 계산 및 가중치 적용
        - Total Score 관리
    - **UI/HUD**
        - Score 실시간 표시
        - 게임 오버 위젯에서 Total Score 표시
- **라운드 타이머 증가**
    - **게임모드 / GameMode**
        - 라운드 시작 시 제한 시간 재설정
        - 타이머 종료 시 라운드 종료 / 게임오버 처리
    - **UI/HUD**
        - 남은 시간 표시
        - 타이머 갱신 시 HUD 업데이트
        - 게임 오버 처리 시 UI 호출
- **미션**
    - **게임모드 / GameMode**
        - “N명 처치” 목표 관리
        - 목표 달성 시 무기 보상 지급
    - **UI/HUD**
        - 미션 진행 상태 표시 (현재 처치 수 / 목표 수)
- **재장전 파이프라인(Delegates-First)**
    - **캐릭터 / Character**
        - 재장전 애니메이션 플레이 (몽타주), 시퀀스, 로코모션 구현
    - **총기 / Weapon**
        - 총기 애니메이션 구현
        - 탄약 관리 및 재장전 로직 구현
        - Reload 델리게이트 브로드캐스트 → 누가 구독하고 있는지 알림
    - **UI / HUD**
        - 델리게이트 구독 → 현재 무기명, 탄약 수 표시
        - 무기 교체 시 즉시 탄약 갱신
 
<br>

## 🔁 Game Flow Chart

![Game Flow Chart](GameFlowChart.png)

1. 게임 시작 & 메인 메뉴
 - 게임 실행 후 메인 메뉴가 나타난다.
 - 플레이어는 여기서 Game Start를 눌러 본격적인 플레이를 시작한다.

2. Level 1 입장
 - 첫 번째 스테이지는 튜토리얼 성격을 가진다.
 - 이동, 사격, 점수 시스템, 미션 규칙 등을 자연스럽게 배우게 된다.

3. 전투 진행
 - 적을 처치하면 점수 획득과 함께 게임모드가 이를 관리한다.
 - 전투 중 다양한 이벤트가 발생한다:
   - 총기 획득 / 업그레이드 (10마리 처치 시 무기 강화)
   - 퀘스트 지급 (“N명 처치” 미션 제공)
   - 점수 누적 (Total Score에 반영)
   - 시간 제한 관리 (시간 안에 목표 달성 못 하면 게임오버)

4. Level 2 입장 조건
 - 주어진 시간 안에 목표를 달성하면 Level 2로 진입.
 - 실패할 경우 게임 종료(Game Over) 처리.

5. Level 2 진행
 - 본격적인 서바이벌 라운드 시스템이 시작된다.
 - Round 1 → Round 2 → Round 3 순으로 진행되며, 라운드마다 제한 시간과 적 처치 목표가 갱신된다.

6. 엔딩 & 점수 집계
 - 라운드가 모두 끝나면 엔딩 메뉴가 열리며, 지금까지 누적된 최종 Total Score를 확인할 수 있다.

7. 게임 종료
 - 엔딩 메뉴를 확인한 뒤 게임은 종료된다.

<br>

## 🎥 시연 영상

🔗 [YouTube](https://www.youtube.com/watch?v=nflci2Y7M5w)

<br>

## 🛠 트러블 슈팅
- [Git Branch 용량 낭비 문제](https://www.notion.so/kimyeoul/Git-Branch-253cf60eefb681498d3ad575fac1298f)
- [브랜치 병합 방식 문제](https://www.notion.so/kimyeoul/253cf60eefb68120b7effad2ee4a9b8a)
- [두 개의 라운드 종료 조건 통합 문제](https://www.notion.so/kimyeoul/253cf60eefb681f082c5de85c8497173)
- [AI 행동 로직(비헤이비어 트리) 문제](https://www.notion.so/kimyeoul/AI-253cf60eefb68126b8beee1b9ec23dc6)
- [AI 총알 피격 콜리전 문제](https://www.notion.so/kimyeoul/AI-253cf60eefb681d2ae82c0210202a341)
- [발사 기능 구현 문제](https://www.notion.so/kimyeoul/253cf60eefb68143a8ecee7f0a3dbc7a)
- [무기 줍기 및 장착 기능 구현 문제](https://www.notion.so/kimyeoul/253cf60eefb68181a2bad2fcdcbfd8bc)
- [1인칭 및 3인칭 카메라 생성 문제](https://www.notion.so/kimyeoul/1-3-253cf60eefb68177891fdaaf7f5dc002)
- [무기 시스템 UI 및 데이터 연결 문제](https://www.notion.so/kimyeoul/UI-253cf60eefb6812c8a65d05070faa188)
- [게임 오버 처리 델리게이트 문제](https://www.notion.so/kimyeoul/253cf60eefb6819f9a58d0d05360c71f)
- [벽을 쏴도 히트마커가 나오는 문제](https://www.notion.so/kimyeoul/253cf60eefb6817c8b3feee059015662)

<br>

## 🧾 노션
- [PPP](https://www.notion.so/kimyeoul/PPP-Purge-Protocol-Project-253cf60eefb680909926f00d4ccbdf62)

<br>

## 👥 역할 

| 담당자 | 담당 역할 |
|-----------|--------|
| 김기탁 | 게임 모드 / 깃•에셋 관리 / 스테이지 제작 / 상호작용 액터 / 레벨 디자인 |
| 김상범 | AI 캐릭터 / 몬스터 웨이브 / PPT |
| 김여울 | UI 시스템 (OutGame UI 및 Weapon UI) / 노션 / ReadMe / 발표 |
| 성준모 | 무기 발사 및 데미지 처리 / 시연 영상 / 발표 대본 |
| 정현성| UI 시스템 (InGame UI : HUD 및 전투 UI) / 노션 / 플로우 차트 |
| 정현수 | 캐릭터 이동 및 상태 변화 / 시연 영상 편집 / 레벨 디자인 |

<br>


## ⚙️ 사용한 툴

- [Rider](https://www.jetbrains.com/ko-kr/rider/) – 개발 환경
- [Team Notion](https://www.notion.so/teamsparta/21b2dc3ef5148023b05ecd2333dedc16) – 기획, 정리
- [draw.io](https://app.diagrams.net/) – Game Flow Chart 제작
- [pixabay](https://pixabay.com/) - 배경 음악
- [Google Fonts](https://fonts.google.com/specimen/VT323) - 폰트

<br>

## 🧑‍🤝‍🧑 4조 팀원

| 이름 | 이메일 | GitHub |
|------|--------|--------|
| 김기탁 | alaalxk@gmail.com | [@mimmita1032](https://github.com/mimmita1032) |
| 김상범 | wb0soon0@gmail.com | [@Kim0430-sa](https://github.com/Kim0430-sa?tab=repositories) |
| 김여울 | yoohozzy@gmail.com| [@yeoulkim](https://github.com/yeoulkim) |
| 성준모 | sjm7829@naver.com | [@komokomo123](https://github.com/komokomo123?tab=repositories) |
| 정현성 | gustjd0415_@naver.com | [@gustjd0415](https://github.com/gustjd0415) |
| 정현수 | freedomquest88@naver.com | [@jhs10507](https://github.com/jhs10507) |