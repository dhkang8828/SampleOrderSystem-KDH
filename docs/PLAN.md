# PLAN.md - 구현 계획

**프로젝트**: 반도체 시료 생산주문관리 시스템  
**작성일**: 2026-06-12  
**개발 방식**: TDD (Red → Green → Refactor)

---

## 전체 Phase 구성

```
Phase 0 │ 프로젝트 환경 설정
Phase 1 │ Util Layer        (DBManager, IdGenerator, DateTimeUtil)
Phase 2 │ Model Layer       (Sample, Order, ProductionJob, enums)
Phase 3 │ Repository Layer  (IRepository<T>, 3개 구현체)
Phase 4 │ Controller Layer  (4개 Controller)
Phase 5 │ View Layer        (6개 View)
Phase 6 │ 통합 및 Main 연결
Phase 7 │ 최종 검증 및 정리
```

---

## Phase 0 - 프로젝트 환경 설정

**목표**: 빌드 환경과 디렉토리 구조 완성

### 작업 목록
- [ ] 디렉토리 구조 생성 (`src/model`, `src/controller`, `src/view`, `src/repository`, `src/util`, `tests/`, `data/`)
- [ ] `CMakeLists.txt` 작성 (C++17, sqlite3, Google Test 링크)
- [ ] Google Test 설치 확인 및 빌드 테스트
- [ ] sqlite3 라이브러리 확인 및 링크 테스트
- [ ] `main.cpp` 스켈레톤 생성
- [ ] 더미 테스트 1개 작성 후 빌드/실행 확인

### 완료 기준
```
cmake --build . && ./run_tests 실행 시 테스트 1개 PASS
```

### 커밋 단위
```
chore: 프로젝트 빌드 환경 설정 (CMake, sqlite3, GTest)
```

---

## Phase 1 - Util Layer

**목표**: 전체 레이어가 공통으로 사용하는 유틸리티 구현  
**순서**: DBManager → DateTimeUtil → IdGenerator

### 1-1. DBManager
- [ ] `DBManager` 클래스 헤더 작성 (`src/util/DBManager.h`)
- [ ] `data/` 폴더 자동 생성 로직 구현
- [ ] SQLite 연결/해제 구현
- [ ] `createTables()` 구현 (3개 테이블 CREATE IF NOT EXISTS)
- [ ] 프로그램 재실행 시 기존 DB 유지 확인

### 1-2. DateTimeUtil
- [ ] `now()` - 현재 시각 ISO 8601 반환
- [ ] `today()` - 오늘 날짜 YYYYMMDD 반환
- [ ] 테스트 작성 및 통과

### 1-3. IdGenerator
- [ ] `generateOrderId(sequence)` - ORD-YYYYMMDD-NNNN
- [ ] `generateSampleId(sequence)` - S-NNN
- [ ] 테스트 작성 및 통과

### 완료 기준
```
- DB 파일 생성 및 3개 테이블 초기화 확인
- 재실행 시 기존 데이터 유지 확인
- IdGenerator, DateTimeUtil 단위 테스트 PASS
```

### 커밋 단위
```
feat: DBManager SQLite 연결 및 테이블 초기화 구현
feat: DateTimeUtil, IdGenerator 유틸리티 구현
test: Util 레이어 단위 테스트 작성
```

---

## Phase 2 - Model Layer

**목표**: 도메인 모델 및 enum 정의  
**순서**: enum → Sample → Order → ProductionJob

### 2-1. Enum 정의
- [ ] `OrderStatus` enum class 정의
- [ ] `StockStatus` enum class 정의
- [ ] enum ↔ string 변환 함수 작성

### 2-2. Sample
- [ ] `Sample` 클래스 헤더/구현 작성
- [ ] `hasEnoughStock()`, `getShortfall()`, `getStockStatus()` 구현
- [ ] 단위 테스트 작성 및 통과

### 2-3. Order
- [ ] `Order` 클래스 헤더/구현 작성
- [ ] 상태 전이 메서드(`isReserved()` 등) 구현
- [ ] 단위 테스트 작성 및 통과

### 2-4. ProductionJob
- [ ] `ProductionJob` 클래스 헤더/구현 작성
- [ ] 단위 테스트 작성 및 통과

### 완료 기준
```
Model 레이어 전체 단위 테스트 PASS
```

### 커밋 단위
```
feat: OrderStatus, StockStatus enum 정의
feat: Sample 도메인 모델 구현
feat: Order 도메인 모델 구현
feat: ProductionJob 도메인 모델 구현
test: Model 레이어 단위 테스트 작성
```

---

## Phase 3 - Repository Layer

**목표**: SQLite 기반 CRUD 영속성 구현  
**순서**: IRepository → SampleRepository → OrderRepository → ProductionQueueRepository

### 3-1. IRepository\<T\>
- [ ] 추상 인터페이스 헤더 작성

### 3-2. SampleRepository
- [ ] `save`, `findById`, `findAll`, `update`, `remove` 구현
- [ ] `findByName`, `updateStock`, `existsById` 구현
- [ ] 단위 테스트 작성 및 통과 (테스트용 인메모리 DB 사용)

### 3-3. OrderRepository
- [ ] `save`, `findById`, `findAll`, `update`, `remove` 구현
- [ ] `findByStatus`, `updateStatus`, `countByStatus` 구현
- [ ] `generateNextOrderSequence` 구현
- [ ] 단위 테스트 작성 및 통과

### 3-4. ProductionQueueRepository
- [ ] IScheduler 인터페이스 정의
- [ ] `save`, `findAll`, `remove` 구현
- [ ] `next()`, `enqueue()`, `getQueue()` FIFO 구현
- [ ] `findByOrderId`, `removeByOrderId` 구현
- [ ] 단위 테스트 작성 및 통과

### 완료 기준
```
- 각 Repository CRUD 단위 테스트 PASS
- 프로그램 재실행 후 데이터 유지 확인
```

### 커밋 단위
```
feat: IRepository<T> 추상 인터페이스 정의
feat: SampleRepository SQLite CRUD 구현
feat: OrderRepository SQLite CRUD 구현
feat: ProductionQueueRepository FIFO 큐 구현
test: Repository 레이어 단위 테스트 작성
```

---

## Phase 4 - Controller Layer

**목표**: 핵심 비즈니스 로직 구현  
**순서**: SampleController → OrderController → ProductionController → ReleaseController

### 4-1. SampleController
- [ ] `registerSample` 구현 (중복 체크, 유효성 검사)
- [ ] `getAllSamples`, `getSampleById`, `searchSamplesByName` 구현
- [ ] `increaseStock`, `decreaseStock` 구현
- [ ] 단위 테스트 작성 및 통과

### 4-2. OrderController
- [ ] `placeOrder` 구현 (주문 접수, RESERVED)
- [ ] `approveOrder` 구현 (재고 확인 → CONFIRMED or PRODUCING 분기)
- [ ] `rejectOrder` 구현 (REJECTED)
- [ ] `getOrderCountByStatus` 구현
- [ ] 단위 테스트 작성 및 통과 **(핵심 테스트)**

### 4-3. ProductionController
- [ ] `getCurrentJob`, `getWaitingJobs` 구현
- [ ] `completeCurrentJob` 구현 (재고 증가 + CONFIRMED 전환)
- [ ] `calcActualProductionQty` static 메서드 구현
- [ ] `calcTotalProductionTime` static 메서드 구현
- [ ] 단위 테스트 작성 및 통과 **(생산량 계산 공식 검증)**

### 4-4. ReleaseController
- [ ] `getConfirmedOrders` 구현
- [ ] `releaseOrder` 구현 (RELEASED 전환)
- [ ] 단위 테스트 작성 및 통과

### 완료 기준
```
- Controller 전체 단위 테스트 PASS
- 주문 승인 시 재고 충분/부족 분기 정확히 동작
- 생산량 계산 공식 ceil(부족분 / (수율 * 0.9)) 검증
```

### 커밋 단위
```
feat: SampleController 구현
feat: OrderController 주문 접수/승인/거절 구현
feat: ProductionController 생산 라인 관리 구현
feat: ReleaseController 출고 처리 구현
test: Controller 레이어 단위 테스트 작성
```

---

## Phase 5 - View Layer

**목표**: 콘솔 UI 구현  
**순서**: MainView → SampleView → OrderView → ApprovalView → MonitoringView → ProductionView → ReleaseView

### 작업 목록
- [ ] `MainView` - 헤더, 현황 요약, 메뉴 선택
- [ ] `SampleView` - 시료 등록, 목록, 검색 화면
- [ ] `OrderView` - 주문 접수 폼 및 결과 출력
- [ ] `ApprovalView` - RESERVED 목록, 승인/거절 처리 화면
- [ ] `MonitoringView` - 상태별 주문 수, 재고 현황 출력
- [ ] `ProductionView` - 생산 현황, 대기 큐 출력
- [ ] `ReleaseView` - 출고 가능 목록, 출고 처리 결과 출력

### 완료 기준
```
- 각 메뉴 진입 및 [0] 뒤로 동작 확인
- 잘못된 입력에 대한 오류 메시지 출력
- 모든 화면이 Controller를 통해서만 데이터 접근
```

### 커밋 단위
```
feat: MainView 메인 메뉴 화면 구현
feat: SampleView 시료 관리 화면 구현
feat: OrderView, ApprovalView 주문 화면 구현
feat: MonitoringView 모니터링 화면 구현
feat: ProductionView 생산라인 화면 구현
feat: ReleaseView 출고 처리 화면 구현
```

---

## Phase 6 - 통합 및 Main 연결

**목표**: 모든 레이어를 연결하여 완전한 애플리케이션 완성

### 작업 목록
- [ ] `main.cpp` 완성
  - DBManager 초기화
  - Repository 인스턴스 생성
  - Controller 인스턴스 생성 (Repository 주입)
  - View 인스턴스 생성 (Controller 주입)
  - MainView 루프 시작
- [ ] 전체 흐름 통합 테스트
  - 시료 등록 → 주문 접수 → 승인(재고 충분) → 출고
  - 시료 등록 → 주문 접수 → 승인(재고 부족) → 생산 완료 → 출고
  - 주문 거절 흐름
- [ ] 프로그램 종료 후 재실행 시 데이터 유지 확인

### 완료 기준
```
- 전체 시나리오 수동 테스트 PASS
- 재실행 후 이전 데이터 유지 확인
```

### 커밋 단위
```
feat: main.cpp 전체 레이어 연결 및 애플리케이션 완성
test: 전체 흐름 통합 시나리오 검증
```

---

## Phase 7 - 최종 검증 및 정리

**목표**: 코드 품질 점검 및 문서 최종화

### 작업 목록
- [ ] 전체 테스트 실행 및 PASS 확인
- [ ] CleanCode 점검 (네이밍, 단일 책임, 중복 제거)
- [ ] 예외 처리 및 경계값 점검 (0 수량, 존재하지 않는 ID 등)
- [ ] `CLAUDE.md`, `PRD.md` 최종 업데이트
- [ ] `README.md` 작성 (빌드/실행 방법)
- [ ] 최종 커밋 및 푸시

### 완료 기준
```
- 전체 단위 테스트 PASS
- 빌드 경고 없음
- README.md 작성 완료
```

### 커밋 단위
```
refactor: 코드 정리 및 CleanCode 적용
docs: README.md 작성
chore: 최종 정리
```

---

## 요약

| Phase | 내용 | 주요 산출물 |
|---|---|---|
| 0 | 환경 설정 | CMakeLists.txt, 디렉토리 구조 |
| 1 | Util Layer | DBManager, IdGenerator, DateTimeUtil |
| 2 | Model Layer | Sample, Order, ProductionJob |
| 3 | Repository Layer | SQLite CRUD 구현체 3개 |
| 4 | Controller Layer | 비즈니스 로직 4개 **(핵심)** |
| 5 | View Layer | 콘솔 UI 7개 |
| 6 | 통합 | main.cpp, 전체 연결 |
| 7 | 검증/정리 | 테스트 PASS, README |
