# NBC_BaseBallGame

내일배움캠프 멀티플레이 9번 과제 레포지토리입니다. 13조 권기문

UE 5.5 기반 **멀티플레이어 숫자야구(Number Baseball)** 게임입니다.

---

## 시연 영상

[![시연 영상](https://img.youtube.com/vi/hfw7xgtGX7o/0.jpg)](https://youtu.be/hfw7xgtGX7o?si=nJktBX-pL5UkhYTo)

---

## Content 파일 (Git 미포함)

에셋·블루프린트 등 Content 디렉터리는 용량 문제로 Git에 포함되어 있지 않습니다.

[Google Drive에서 다운로드](https://drive.google.com/file/d/1DkXmQJ1U5-ET5NxSrlFUjgwuXOtGrM62/view?usp=drive_link)

---

## 게임 규칙

- 각 플레이어는 중복 없는 N자리 숫자를 비밀 번호로 설정합니다.
- 상대방의 숫자를 추측하면 **Strike / Ball / Out** 피드백을 받습니다.
  - **Strike:** 숫자와 자리 모두 일치
  - **Ball:** 숫자는 맞지만 자리 불일치
  - **Out:** 숫자가 하나도 없음
- 제한 횟수 안에 정확히 맞히면 승리합니다.

---

## 동작 흐름

```
[로비]
  └─ 모든 플레이어가 "준비 완료" 버튼을 누르면 게임 시작

[비밀 번호 설정]
  └─ 각 플레이어가 중복 없는 숫자를 입력하여 자신의 비밀 번호 확정

[턴제 진행]
  └─ 플레이어 순서대로 추측 입력
  └─ 제한 시간 초과 시 해당 턴의 기회 1회 자동 차감 후 다음 턴으로 이동
  └─ 서버가 Strike/Ball/Out 판정 후 전체 클라이언트에 브로드캐스트

[종료]
  └─ 정답 맞힘 → 승리/패배 위젯 표시
  └─ 기회 소진 → 패배 처리
  └─ 재도전 요청 수락 시 로비로 복귀
```

---

## 아키텍처 개요

| 클래스 | 역할 |
|---|---|
| `BBGameMode` | 서버 전용. 게임 흐름 제어(시작·턴 관리·승패 판정) |
| `BBGameState` | 전체 클라이언트에 복제되는 공용 게임 상태 |
| `BBPlayerState` | 플레이어별 준비 상태·점수 복제 |
| `BBPlayerController` | 클라이언트 입력 수신 및 서버 RPC 호출 |
| `BBChatWidget` | 입력 UI. 게임 단계(로비/게임/종료)에 따라 버튼 동작 분기 |
| `BBChatMessageEntry` | 채팅·피드백 메시지 한 줄 표시 위젯 |
| `BBScoreboardWidget` | 남은 기회·턴 타이머 시각화 |

- **Dedicated Server** 구조로 설계되었습니다.
- 모든 게임 로직 판정은 서버에서만 수행하고, 결과는 RPC / Replication으로 클라이언트에 전달합니다.
- 입력 시스템은 **Enhanced Input** 을 사용합니다.

---

## 빌드 환경

- **Engine:** Unreal Engine 5.5
- **Platform:** Win64
- **Build:** Unreal Build Tool (UBT)
