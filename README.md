# SampleOrderSystem

반도체 회사 **S-Semi**의 시료(Sample) 생산주문관리 콘솔 애플리케이션.  
주문 접수부터 생산, 출고까지의 전체 흐름을 관리한다.

---

## 기술 스택

| 항목 | 내용 |
|---|---|
| 언어 | C++17 |
| 아키텍처 | MVC 패턴 |
| 데이터 영속성 | SQLite3 (amalgamation) |
| 테스트 | Google Test (FetchContent) |
| 빌드 | CMake + NMake (MSVC) |
| 실행 방식 | 콘솔(Console) |

---

## 디렉토리 구조

```
SampleOrderSystem/
├── main.cpp
├── src/
│   ├── model/          # 도메인 모델 (Sample, Order, ProductionJob)
│   ├── controller/     # 비즈니스 로직 (SampleController, OrderController, ReleaseController)
│   ├── view/           # 콘솔 화면 (MainView, SampleView, OrderView ...)
│   ├── repository/     # SQLite CRUD (SampleRepository, OrderRepository, ProductionQueueRepository)
│   └── util/           # 공통 유틸 (DBManager, IdGenerator, DateTimeUtil, ConsoleUtil)
├── tests/
│   ├── model/          # 도메인 모델 단위 테스트
│   ├── controller/     # 비즈니스 로직 단위 테스트
│   ├── repository/     # DB CRUD 단위 테스트
│   ├── util/           # 유틸리티 단위 테스트
│   └── integration/    # 통합 테스트 (22개)
├── data/               # SQLite DB 파일 저장 위치 (sample_order.db)
└── docs/               # PRD, 클래스 설계 문서
```

---

## 주문 상태 흐름

```
RESERVED (접수)
  ├─ 거절 → REJECTED
  └─ 승인
       ├─ 재고 충분 → CONFIRMED (출고 대기) → RELEASED (출고 완료)
       └─ 재고 부족 → PRODUCING (생산 중) → CONFIRMED → RELEASED
```

---

## 빌드 및 실행

### 환경 요구사항
- Visual Studio 2026 (MSVC)
- CMake 3.20 이상

### 최초 1회 설정

```bat
:: MSVC x64 환경 초기화
"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

:: CMake 설정
cmake -S . -B build -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Debug
```

### 빌드

```bat
cd build
nmake
```

### 실행

```bat
build\SampleOrderSystem.exe
```

### 테스트 실행

```bat
build\run_tests.exe
```

---

## 메뉴 구성 및 사용법

프로그램 실행 시 메인 메뉴가 표시된다.  
상단에는 시스템 현황 요약(등록 시료 수, 총 재고, 전체 주문 수, 생산 대기 수)이 표시된다.

```
[1] 시료 관리
[2] 시료 주문
[3] 주문 승인/거절
[4] 모니터링
[5] 생산 라인
[6] 출고 처리
[0] 종료
```

### [1] 시료 관리
- `1` 시료 등록: 이름, 평균 생산시간(min/ea), 수율(0~1), 초기 재고 입력
- `2` 시료 목록 조회: 등록된 시료 전체 + 재고 수량 표시
- `3` 시료 검색: ID로 특정 시료 조회

### [2] 시료 주문
- 시료 ID, 고객명, 주문 수량 입력 후 확인(Y/N)
- 접수 완료 시 주문번호(ORD-YYYYMMDD-NNNN) 발급, 상태 `RESERVED`

### [3] 주문 승인/거절
- `RESERVED` 상태 주문 목록 표시
- 주문 선택 후:
  - **승인(1)**: 재고 충분 → `CONFIRMED`, 재고 부족 → 생산 라인 자동 등록 + `PRODUCING`
  - **거절(2)**: 즉시 `REJECTED`

### [4] 모니터링
- 상태별 주문 수 (`RESERVED` / `CONFIRMED` / `PRODUCING` / `RELEASED`, `REJECTED` 제외)
- 시료별 재고 현황: **여유** (재고 충분) / **부족** (재고 < 대기 주문량) / **고갈** (재고 0)

### [5] 생산 라인 (조회 전용)
- 현재 생산 중인 주문 정보 + 경과 시간 / 잔여 시간 표시
- 하단에 대기 중인 주문 목록 표시
- 생산 완료는 자동 처리 (메인 화면 전환 시 경과 시간 확인 후 자동으로 `CONFIRMED` 전환)

### [6] 출고 처리
- `CONFIRMED` 상태 주문 목록 표시
- 주문 선택 후 출고 실행 → `RELEASED`, 재고에서 수량 차감

---

## 생산 공식

```
실 생산량 = ceil(부족분 / (수율 × 0.9))
총 생산시간 = 평균 생산시간 × 실 생산량
```

- 생산 라인은 단일 라인, **FIFO** 스케줄링
- 재고 차감 시점: 출고(`[6]`)에서만 1회 차감

---

## 숨김 커맨드

메인 메뉴 선택 프롬프트에서 입력:

| 키워드 | 동작 |
|---|---|
| `DUMMY` | 랜덤 시료 3개 + 주문 6개 자동 삽입 (테스트용) |
| `RESET` | 전체 DB 초기화 (`CONFIRM` 입력으로 최종 확인) |

---

## 테스트 현황

| 분류 | 테스트 수 |
|---|---|
| 모델 단위 테스트 | 14개 |
| 컨트롤러 단위 테스트 | 18개 |
| 레포지토리 단위 테스트 | 24개 |
| 유틸리티 단위 테스트 | 16개 |
| 통합 테스트 | 22개 |
| **합계** | **94개** |

통합 테스트는 `:memory:` SQLite를 사용하여 격리된 환경에서 실행된다.
