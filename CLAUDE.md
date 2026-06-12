# 반도체 시료 생산주문관리 시스템 (SampleOrderSystem)

## 프로젝트 개요

가상의 반도체 회사 "S-Semi"의 시료(Sample) 생산주문관리 콘솔 애플리케이션.
주문 접수부터 생산, 출고까지의 전체 흐름을 관리한다.

## 기술 스택

- 언어: C++ (C++17)
- 아키텍처: MVC 패턴 (Model / Controller / View 디렉토리 분리)
- 데이터 영속성: SQLite (sqlite3 라이브러리)
- 테스트 프레임워크: Google Test (gtest)
- 실행 방식: 콘솔(Console) 기반

## 디렉토리 구조

```
src/
├── model/          # 도메인 모델 (Sample, Order, ProductionLine 등)
├── controller/     # 비즈니스 로직 처리
├── view/           # 콘솔 입출력 화면
├── repository/     # 데이터 영속성 처리 (SQLite CRUD)
└── util/           # 공통 유틸리티
tests/              # Google Test 단위 테스트
data/               # SQLite DB 파일 저장 위치 (sample_order.db)
```

## 도메인 모델

### Sample (시료)
| 필드 | 타입 | 설명 |
|---|---|---|
| sampleId | String | 시료 고유 ID (예: S-001) |
| name | String | 시료 이름 |
| avgProductionTime | double | 평균 생산시간 (min/ea) |
| yield | double | 수율 (0~1, 예: 0.92) |
| stock | int | 현재 재고 수량 |

### Order (주문)
| 필드 | 타입 | 설명 |
|---|---|---|
| orderId | String | 주문번호 (예: ORD-20260612-0001) |
| sampleId | String | 주문한 시료 ID |
| customerName | String | 고객명 |
| quantity | int | 주문 수량 |
| status | OrderStatus | 주문 상태 |
| createdAt | string | 주문 생성일시 (ISO 8601 형식) |

### OrderStatus (주문 상태)
| 상태 | 의미 |
|---|---|
| RESERVED | 주문 접수 |
| REJECTED | 주문 거절 (모니터링 제외) |
| PRODUCING | 주문 승인 완료 + 재고 부족으로 생산 중 |
| CONFIRMED | 주문 승인 완료 + 출고 대기 중 |
| RELEASED | 출고 완료 |

## 주문 상태 흐름

```
RESERVED
  ├─ 거절 → REJECTED
  └─ 승인
       ├─ 재고 충분 → CONFIRMED → RELEASED
       └─ 재고 부족 → PRODUCING → (생산 완료) → CONFIRMED → RELEASED
```

## 기능 명세

### 1. 메인 메뉴
- 시스템 현황 요약 표시 (등록 시료 수, 총 재고, 전체 주문 수, 생산라인 대기 수)
- 메뉴 선택: [1] 시료 관리 / [2] 시료 주문 / [3] 주문 승인·거절 / [4] 모니터링 / [5] 생산라인 조회 / [6] 출고 처리 / [0] 종료

### 2. 시료 관리
- **시료 등록**: sampleId, 이름, 평균 생산시간, 수율 입력
- **시료 목록 조회**: 등록된 모든 시료 + 현재 재고 수량 표시
- **시료 검색**: 이름 등 속성으로 특정 시료 검색

### 3. 시료 주문
- 입력: 시료 ID, 고객명, 주문 수량
- 입력 내용 확인 후 [Y] 예약 접수 / [N] 취소
- 접수 완료 시 orderId 발급, 상태 RESERVED

### 4. 주문 승인/거절
- RESERVED 상태 주문 목록 표시
- 특정 주문 선택 후:
  - **승인**: 재고 확인
    - 재고 충분 → 즉시 CONFIRMED
    - 재고 부족 → 생산 라인 자동 등록, PRODUCING
  - **거절**: 즉시 REJECTED

### 5. 모니터링
- **주문량 확인**: 상태별(RESERVED / CONFIRMED / PRODUCING / RELEASED) 주문 수 표시 (REJECTED 제외)
- **재고량 확인**: 시료별 재고 수량 + 상태 표기
  - 여유: 주문 대비 재고 충분
  - 부족: 주문 대비 재고 부족
  - 고갈: 수량 0

### 6. 생산 라인
- 스케줄링: FIFO (선입선출)
- **실 생산량**: `ceil(부족분 / (수율 × 0.9))`
- **총 생산 시간**: `평균 생산시간 × 실 생산량`
- 생산 완료 시 주문 상태 PRODUCING → CONFIRMED
- **생산 현황**: 현재 생산 중인 시료 정보 (주문 정보, 진행 현황 등)
- **대기 주문 확인**: 생산 큐의 대기 목록 출력

### 7. 출고 처리
- CONFIRMED 상태 주문 목록 표시
- 특정 주문 선택 후 출고 실행
- 주문 상태 CONFIRMED → RELEASED

## 개발 주안점 (평가 기준)

1. **문서 관리**: CLAUDE.md, PRD.md 등 문서 유지
2. **Harness 도입**: Claude Code 설정 활용
3. **Test**: 주요 비즈니스 로직 단위 테스트
4. **CleanCode**: 명확한 네이밍, 단일 책임, 중복 제거
5. **Commit 이력**: 기능 단위로 의미 있는 커밋

## 빌드 및 실행

```bash
# 컴파일 (sqlite3 링크 포함)
g++ -std=c++17 -o SampleOrderSystem src/**/*.cpp -lsqlite3

# 실행
./SampleOrderSystem
```

## TDD 개발 원칙

### 사이클
1. **Red**: 실패하는 테스트 먼저 작성
2. **Green**: 테스트를 통과하는 최소한의 구현
3. **Refactor**: 코드 정리 (테스트는 계속 통과 유지)

### 테스트 대상 (우선순위 순)
- `OrderController`: 승인/거절 로직, 재고 판단, 상태 전환
- `ProductionController`: 실 생산량 계산 `ceil(부족분 / (수율 × 0.9))`, FIFO 큐 동작
- `SampleRepository`: SQLite CRUD (등록, 조회, 재고 업데이트)
- `OrderRepository`: SQLite CRUD, 상태별 조회

### 테스트 빌드 및 실행

```bash
# Google Test 포함 컴파일
g++ -std=c++17 -o run_tests tests/**/*.cpp src/**/*.cpp -lsqlite3 -lgtest -lgtest_main -lpthread

# 테스트 실행
./run_tests
```

### 테스트 파일 네이밍
- `tests/model/` → 도메인 모델 테스트
- `tests/controller/` → 비즈니스 로직 테스트
- `tests/repository/` → DB CRUD 테스트

## 주의사항

- 모델 사용: Claude Sonnet / Effort Medium
- 시스템에 등록된 시료만 주문 가능
- 생산 라인은 단일 라인 (하나씩 순차 처리)
- REJECTED 주문은 모니터링 및 통계에서 제외
