# TEST.md - 테스트 계획

**프로젝트**: 반도체 시료 생산주문관리 시스템  
**작성일**: 2026-06-12  
**프레임워크**: Google Test (gtest)  
**원칙**: TDD - 구현 전 테스트 먼저 작성 (Red → Green → Refactor)

---

## 테스트 파일 구조

```
tests/
├── util/
│   ├── test_DBManager.cpp
│   ├── test_DateTimeUtil.cpp
│   └── test_IdGenerator.cpp
├── model/
│   ├── test_Sample.cpp
│   ├── test_Order.cpp
│   └── test_ProductionJob.cpp
├── repository/
│   ├── test_SampleRepository.cpp
│   ├── test_OrderRepository.cpp
│   └── test_ProductionQueueRepository.cpp
└── controller/
    ├── test_SampleController.cpp
    ├── test_OrderController.cpp
    ├── test_ProductionController.cpp
    └── test_ReleaseController.cpp
```

> View 레이어는 콘솔 I/O 특성상 단위 테스트 제외 → 수동 테스트로 검증

---

## 1. Util Layer 테스트

### 1-1. DBManager (`test_DBManager.cpp`)

**검증 목표**: DB 연결, 테이블 초기화, 데이터 영속성

| 테스트명 | 검증 내용 | 방법 |
|---|---|---|
| `Initialize_CreatesDBFile` | initialize() 호출 시 DB 파일 생성 | 파일 존재 여부 확인 |
| `Initialize_CreatesDataDirectory` | data/ 폴더 없어도 자동 생성 | 폴더 삭제 후 재실행 |
| `Initialize_CreatesTables` | 3개 테이블 생성 확인 | sqlite_master 조회 |
| `Initialize_ExistingDB_PreservesData` | 재실행 시 기존 데이터 유지 | 데이터 삽입 후 재연결 |
| `GetConnection_ReturnsValidPointer` | 연결 후 커넥션 포인터 유효 | nullptr 여부 확인 |
| `Close_DisconnectsDB` | close() 후 isConnected() false | 상태 확인 |

```cpp
TEST(DBManagerTest, Initialize_CreatesDataDirectory) {
    std::filesystem::remove_all("data");
    EXPECT_FALSE(std::filesystem::exists("data"));
    DBManager::getInstance().initialize("data/test.db");
    EXPECT_TRUE(std::filesystem::exists("data"));
}
```

---

### 1-2. DateTimeUtil (`test_DateTimeUtil.cpp`)

**검증 목표**: 날짜/시간 포맷 정확성

| 테스트명 | 검증 내용 | 방법 |
|---|---|---|
| `Now_ReturnsISO8601Format` | 반환값이 ISO 8601 형식인지 | 정규식 매칭 |
| `Today_Returns8DigitString` | 반환값이 YYYYMMDD 형식인지 | 길이 및 숫자 확인 |

```cpp
TEST(DateTimeUtilTest, Now_ReturnsISO8601Format) {
    std::string result = DateTimeUtil::now();
    // 형식: 2026-06-12T09:32:15
    EXPECT_EQ(result.size(), 19);
    EXPECT_EQ(result[4], '-');
    EXPECT_EQ(result[10], 'T');
}
```

---

### 1-3. IdGenerator (`test_IdGenerator.cpp`)

**검증 목표**: ID 생성 형식 및 시퀀스 정확성

| 테스트명 | 검증 내용 | 방법 |
|---|---|---|
| `GenerateOrderId_Format` | ORD-YYYYMMDD-NNNN 형식 | 문자열 파싱 |
| `GenerateOrderId_SequencePadding` | 시퀀스 4자리 0패딩 | sequence=1 → "0001" |
| `GenerateSampleId_Format` | S-NNN 형식 | 문자열 파싱 |
| `GenerateSampleId_SequencePadding` | 3자리 0패딩 | sequence=3 → "S-003" |

```cpp
TEST(IdGeneratorTest, GenerateOrderId_SequencePadding) {
    std::string id = IdGenerator::generateOrderId(1);
    // ORD-20260612-0001 형식 검증
    EXPECT_EQ(id.substr(0, 4), "ORD-");
    EXPECT_EQ(id.substr(13), "0001");
}
```

---

## 2. Model Layer 테스트

### 2-1. Sample (`test_Sample.cpp`)

**검증 목표**: 비즈니스 로직 메서드의 정확한 동작

| 테스트명 | 검증 내용 | 입력 | 기대값 |
|---|---|---|---|
| `HasEnoughStock_True_WhenStockSufficient` | 재고 >= 주문수량 | stock=100, qty=50 | true |
| `HasEnoughStock_True_WhenStockEqual` | 재고 == 주문수량 | stock=50, qty=50 | true |
| `HasEnoughStock_False_WhenStockInsufficient` | 재고 < 주문수량 | stock=30, qty=50 | false |
| `GetShortfall_ReturnsCorrectValue` | 부족분 계산 | stock=30, qty=200 | 170 |
| `GetShortfall_ReturnsZero_WhenSufficient` | 재고 충분 시 0 반환 | stock=200, qty=50 | 0 |
| `GetStockStatus_Depleted_WhenZero` | 재고 0 → DEPLETED | stock=0 | DEPLETED |
| `GetStockStatus_Shortage_WhenInsufficient` | 재고 부족 → SHORTAGE | stock=10, pending=50 | SHORTAGE |
| `GetStockStatus_Sufficient_WhenEnough` | 재고 충분 → SUFFICIENT | stock=100, pending=50 | SUFFICIENT |

```cpp
TEST(SampleTest, GetShortfall_ReturnsZero_WhenSufficient) {
    Sample s("S-001", "테스트시료", 0.5, 0.92, 200);
    EXPECT_EQ(s.getShortfall(50), 0);  // 재고가 충분하면 0
}

TEST(SampleTest, GetStockStatus_Depleted_WhenZero) {
    Sample s("S-001", "테스트시료", 0.5, 0.92, 0);
    EXPECT_EQ(s.getStockStatus(0), StockStatus::DEPLETED);
}
```

---

### 2-2. Order (`test_Order.cpp`)

**검증 목표**: 상태 확인 메서드 및 상태 전이 정확성

| 테스트명 | 검증 내용 | 입력 | 기대값 |
|---|---|---|---|
| `IsReserved_True_WhenStatusReserved` | RESERVED 상태 확인 | status=RESERVED | true |
| `IsConfirmed_False_WhenStatusReserved` | RESERVED에서 isConfirmed() | status=RESERVED | false |
| `SetStatus_ChangesStatus` | setStatus로 상태 변경 | RESERVED→CONFIRMED | CONFIRMED |
| `StatusToString_ReturnsCorrectString` | enum → string 변환 | PRODUCING | "PRODUCING" |

```cpp
TEST(OrderTest, SetStatus_ChangesStatus) {
    Order o("ORD-001", "S-001", "테스트고객", 100, OrderStatus::RESERVED, "2026-06-12T09:00:00");
    o.setStatus(OrderStatus::CONFIRMED);
    EXPECT_TRUE(o.isConfirmed());
    EXPECT_FALSE(o.isReserved());
}
```

---

### 2-3. ProductionJob (`test_ProductionJob.cpp`)

**검증 목표**: 생성자 및 getter 정확성

| 테스트명 | 검증 내용 | 입력 | 기대값 |
|---|---|---|---|
| `Constructor_SetsFieldsCorrectly` | 생성자 필드 설정 | 각 필드값 | 동일한 값 반환 |
| `GetActualProductionQty_ReturnsCorrectValue` | 실 생산량 getter | qty=206 | 206 |

---

## 3. Repository Layer 테스트

> 모든 Repository 테스트는 **인메모리 SQLite DB** (`:memory:`) 사용  
> 각 테스트 전 `setUp`에서 테이블 초기화, 후 `tearDown`에서 정리

### 3-1. SampleRepository (`test_SampleRepository.cpp`)

**검증 목표**: SQLite CRUD 및 추가 조회 메서드

| 테스트명 | 검증 내용 | 방법 |
|---|---|---|
| `Save_InsertsNewSample` | 새 시료 저장 | save 후 findById 확인 |
| `Save_ReturnsFalse_OnDuplicateId` | 중복 ID 저장 시 false | 동일 ID 2회 save |
| `FindById_ReturnsCorrectSample` | ID로 단건 조회 | save 후 findById |
| `FindById_ReturnsNullopt_WhenNotFound` | 없는 ID 조회 | 빈 DB에서 findById |
| `FindAll_ReturnsAllSamples` | 전체 목록 조회 | 3개 저장 후 findAll |
| `Update_UpdatesFields` | 필드 업데이트 | save → update → findById |
| `UpdateStock_UpdatesOnlyStock` | 재고만 업데이트 | updateStock 후 확인 |
| `FindByName_ReturnsMatchingResults` | 이름 부분 검색 | keyword="웨이퍼" |
| `ExistsById_ReturnsTrue_WhenExists` | ID 존재 여부 | save 후 existsById |
| `Remove_DeletesSample` | 시료 삭제 | save → remove → findById |

```cpp
class SampleRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        sqlite3_open(":memory:", &db);
        // 테이블 생성
        dbManager = std::make_unique<DBManager>(db);
        repo = std::make_unique<SampleRepository>(*dbManager);
    }
    void TearDown() override { sqlite3_close(db); }

    sqlite3* db;
    std::unique_ptr<DBManager> dbManager;
    std::unique_ptr<SampleRepository> repo;
};

TEST_F(SampleRepositoryTest, FindById_ReturnsNullopt_WhenNotFound) {
    auto result = repo->findById("S-999");
    EXPECT_FALSE(result.has_value());
}
```

---

### 3-2. OrderRepository (`test_OrderRepository.cpp`)

| 테스트명 | 검증 내용 | 방법 |
|---|---|---|
| `Save_InsertsNewOrder` | 새 주문 저장 | save 후 findById 확인 |
| `FindByStatus_ReturnsCorrectOrders` | 상태별 조회 | RESERVED 2개 저장 후 조회 |
| `UpdateStatus_ChangesOnlyStatus` | 상태만 변경 | updateStatus 후 확인 |
| `CountByStatus_ReturnsCorrectCount` | 상태별 주문 수 | 3개 저장 후 count |
| `FindByStatus_ExcludesOtherStatus` | 다른 상태 제외 | RESERVED 조회 시 CONFIRMED 미포함 |
| `GenerateNextOrderSequence_Increments` | 시퀀스 증가 | 1→2→3 순서 확인 |

---

### 3-3. ProductionQueueRepository (`test_ProductionQueueRepository.cpp`)

| 테스트명 | 검증 내용 | 방법 |
|---|---|---|
| `Enqueue_AddsJobToQueue` | 큐에 작업 추가 | enqueue 후 getQueue 확인 |
| `Next_ReturnsFIFOOrder` | FIFO 순서 반환 | 3개 enqueue 후 next() 순서 |
| `Next_ReturnsNullopt_WhenEmpty` | 빈 큐에서 next() | 빈 큐에서 호출 |
| `RemoveByOrderId_RemovesJob` | 작업 제거 | enqueue → remove → getQueue |
| `GetQueue_ReturnsAllWaitingJobs` | 전체 대기 목록 | 3개 enqueue 후 크기 확인 |

```cpp
TEST_F(ProductionQueueRepositoryTest, Next_ReturnsFIFOOrder) {
    repo->enqueue(ProductionJob(0, "ORD-001", 100, 50.0, "2026-06-12T09:00:00"));
    repo->enqueue(ProductionJob(0, "ORD-002", 200, 100.0, "2026-06-12T09:01:00"));

    auto first = repo->next();
    ASSERT_TRUE(first.has_value());
    EXPECT_EQ(first->getOrderId(), "ORD-001");  // 먼저 들어온 것이 먼저 나옴
}
```

---

## 4. Controller Layer 테스트

> Controller 테스트는 **인메모리 SQLite DB** 기반 Repository를 실제 주입  
> Mock 없이 실제 Repository를 사용하여 통합적 동작 검증

### 4-1. SampleController (`test_SampleController.cpp`)

| 테스트명 | 검증 내용 | 방법 |
|---|---|---|
| `RegisterSample_Success` | 정상 시료 등록 | 등록 후 조회 확인 |
| `RegisterSample_Fail_DuplicateId` | 중복 ID 등록 실패 | 동일 ID 2회 등록 |
| `RegisterSample_Fail_InvalidYieldRate` | 수율 범위 초과 | yieldRate=1.5 |
| `RegisterSample_Fail_ZeroProductionTime` | 생산시간 0 이하 | avgTime=0 |
| `SearchSamplesByName_ReturnsMatches` | 키워드 검색 | "웨이퍼" 검색 |
| `DecreaseStock_Success` | 재고 차감 | stock=100, decrease=30 → 70 |
| `DecreaseStock_Fail_InsufficientStock` | 재고 부족 시 실패 | stock=10, decrease=50 |
| `IncreaseStock_Success` | 재고 증가 | stock=100, increase=50 → 150 |

```cpp
TEST_F(SampleControllerTest, RegisterSample_Fail_InvalidYieldRate) {
    bool result = controller->registerSample("S-001", "테스트", 0.5, 1.5, 100);
    EXPECT_FALSE(result);  // 수율 1.0 초과 → 실패
}
```

---

### 4-2. OrderController (`test_OrderController.cpp`)

**가장 중요한 테스트 - 핵심 비즈니스 로직 검증**

| 테스트명 | 검증 내용 | 방법 |
|---|---|---|
| `PlaceOrder_Success_CreatesReservedOrder` | 주문 접수 → RESERVED | 접수 후 상태 확인 |
| `PlaceOrder_Fail_InvalidSampleId` | 없는 시료 ID로 주문 | nullopt 반환 |
| `PlaceOrder_Fail_ZeroQuantity` | 수량 0으로 주문 | nullopt 반환 |
| `ApproveOrder_Confirmed_WhenStockSufficient` | 재고 충분 → CONFIRMED | stock=200, qty=100 |
| `ApproveOrder_Confirmed_DecreasesStock` | 승인 시 재고 차감 | stock=200, qty=100 → stock=100 |
| `ApproveOrder_Producing_WhenStockInsufficient` | 재고 부족 → PRODUCING | stock=10, qty=100 |
| `ApproveOrder_Producing_EnqueuesProductionJob` | 생산 큐 등록 확인 | 승인 후 큐 확인 |
| `ApproveOrder_Fail_NotReservedStatus` | RESERVED 아닌 주문 승인 시도 | CONFIRMED 주문 승인 |
| `RejectOrder_ChangesStatusToRejected` | 거절 → REJECTED | 거절 후 상태 확인 |
| `RejectOrder_Fail_NotReservedStatus` | RESERVED 아닌 주문 거절 시도 | false 반환 |
| `GetOrderCountByStatus_ReturnsCorrectCounts` | 상태별 주문 수 집계 | 각 상태별 등록 후 확인 |

```cpp
TEST_F(OrderControllerTest, ApproveOrder_Confirmed_WhenStockSufficient) {
    // 시료 등록 (재고 200)
    sampleCtrl->registerSample("S-001", "테스트시료", 0.5, 0.92, 200);
    // 주문 접수 (수량 100)
    auto order = orderCtrl->placeOrder("S-001", "테스트고객", 100);
    ASSERT_TRUE(order.has_value());

    bool result = orderCtrl->approveOrder(order->getOrderId());

    EXPECT_TRUE(result);
    auto updated = orderCtrl->getOrderById(order->getOrderId());
    EXPECT_EQ(updated->getStatus(), OrderStatus::CONFIRMED);

    // 재고 차감 확인
    auto sample = sampleCtrl->getSampleById("S-001");
    EXPECT_EQ(sample->getStock(), 100);  // 200 - 100
}

TEST_F(OrderControllerTest, ApproveOrder_Producing_WhenStockInsufficient) {
    sampleCtrl->registerSample("S-001", "테스트시료", 0.5, 0.92, 10);
    auto order = orderCtrl->placeOrder("S-001", "테스트고객", 100);

    orderCtrl->approveOrder(order->getOrderId());

    auto updated = orderCtrl->getOrderById(order->getOrderId());
    EXPECT_EQ(updated->getStatus(), OrderStatus::PRODUCING);
}
```

---

### 4-3. ProductionController (`test_ProductionController.cpp`)

**생산량 계산 공식 검증이 핵심**

| 테스트명 | 검증 내용 | 입력 | 기대값 |
|---|---|---|---|
| `CalcActualProductionQty_BasicCase` | 기본 생산량 계산 | shortfall=170, yield=0.92 | 206 |
| `CalcActualProductionQty_CeilApplied` | ceil 적용 확인 | shortfall=1, yield=0.92 | 2 |
| `CalcActualProductionQty_ExactDivision` | 나누어 떨어지는 경우 | shortfall=83, yield=1.0 | 93 |
| `CalcTotalProductionTime_IsCorrect` | 총 생산시간 계산 | avgTime=0.8, qty=206 | 164.8 |
| `CompleteCurrentJob_UpdatesStockAndStatus` | 생산 완료 → 재고 증가 + CONFIRMED | 생산 완료 처리 후 확인 |
| `CompleteCurrentJob_RemovesJobFromQueue` | 완료 후 큐에서 제거 | 완료 후 큐 크기 확인 |
| `CompleteCurrentJob_Fail_WhenQueueEmpty` | 빈 큐에서 완료 시도 | false 반환 |
| `GetCurrentJob_ReturnsFirstInQueue` | 현재 작업 = 큐 맨 앞 | FIFO 순서 확인 |
| `GetWaitingJobs_ExcludesCurrentJob` | 대기 목록에 현재 작업 미포함 | 3개 중 2개 반환 |

```cpp
TEST(ProductionControllerTest, CalcActualProductionQty_BasicCase) {
    // ceil(170 / (0.92 * 0.9)) = ceil(205.3) = 206
    int result = ProductionController::calcActualProductionQty(170, 0.92);
    EXPECT_EQ(result, 206);
}

TEST(ProductionControllerTest, CalcActualProductionQty_CeilApplied) {
    // ceil(1 / (0.92 * 0.9)) = ceil(1.207) = 2
    int result = ProductionController::calcActualProductionQty(1, 0.92);
    EXPECT_EQ(result, 2);
}

TEST_F(ProductionControllerTest, CompleteCurrentJob_UpdatesStockAndStatus) {
    // 재고 10, 주문 100 → PRODUCING 상태로 만든 후
    orderCtrl->approveOrder(orderId);

    productionCtrl->completeCurrentJob();

    auto sample = sampleCtrl->getSampleById("S-001");
    EXPECT_GT(sample->getStock(), 10);  // 재고 증가 확인

    auto order = orderCtrl->getOrderById(orderId);
    EXPECT_EQ(order->getStatus(), OrderStatus::CONFIRMED);
}
```

---

### 4-4. ReleaseController (`test_ReleaseController.cpp`)

| 테스트명 | 검증 내용 | 방법 |
|---|---|---|
| `ReleaseOrder_Success_ChangesToReleased` | CONFIRMED → RELEASED | 출고 후 상태 확인 |
| `ReleaseOrder_Fail_NotConfirmedStatus` | CONFIRMED 아닌 주문 출고 | RESERVED 상태 주문 출고 시도 |
| `ReleaseOrder_Fail_InvalidOrderId` | 없는 주문번호 | false 반환 |
| `GetConfirmedOrders_ReturnsOnlyConfirmed` | CONFIRMED만 반환 | 여러 상태 중 CONFIRMED만 |

```cpp
TEST_F(ReleaseControllerTest, ReleaseOrder_Fail_NotConfirmedStatus) {
    // RESERVED 상태 주문 출고 시도
    bool result = releaseCtrl->releaseOrder(reservedOrderId);
    EXPECT_FALSE(result);  // CONFIRMED 아니므로 실패
}
```

---

## 5. 수동 테스트 시나리오 (View 포함 통합)

View 레이어는 콘솔 I/O 특성상 수동으로 검증

### 시나리오 1 - 재고 충분 주문 처리
```
1. 시료 등록: S-001, 실리콘 웨이퍼, 0.5 min/ea, 수율 0.92, 재고 200
2. 주문 접수: S-001, SK하이닉스, 수량 100
3. 주문 승인 → 재고 충분 → 상태 CONFIRMED 확인
4. 출고 처리 → 상태 RELEASED 확인
5. 프로그램 종료 후 재실행 → 데이터 유지 확인
```

### 시나리오 2 - 재고 부족 → 생산 후 출고
```
1. 시료 등록: S-002, SiC 파워기판, 0.8 min/ea, 수율 0.92, 재고 30
2. 주문 접수: S-002, 삼성전자, 수량 200
3. 주문 승인 → 재고 부족 → 생산 큐 등록, 상태 PRODUCING 확인
4. 생산라인 조회 → 큐 대기 확인
5. 생산 완료 처리 → 재고 증가, 상태 CONFIRMED 확인
6. 출고 처리 → 상태 RELEASED 확인
```

### 시나리오 3 - 주문 거절
```
1. 주문 접수 후 → 주문 거절
2. 상태 REJECTED 확인
3. 모니터링 → REJECTED 주문 미표시 확인
```

### 시나리오 4 - 경계값 검증
```
1. 수율 0, 1.1 입력 → 오류 메시지 확인
2. 주문 수량 0 입력 → 오류 메시지 확인
3. 존재하지 않는 시료 ID로 주문 → 오류 메시지 확인
4. 빈 주문 목록에서 승인 시도 → 안내 메시지 확인
```

---

## 테스트 실행 방법

```bash
# 전체 테스트 빌드
g++ -std=c++17 -o run_tests tests/**/*.cpp src/**/*.cpp -lsqlite3 -lgtest -lgtest_main -lpthread

# 전체 테스트 실행
./run_tests

# 특정 테스트만 실행
./run_tests --gtest_filter="OrderControllerTest.*"

# 상세 출력
./run_tests --gtest_output=verbose
```

---

## 테스트 커버리지 목표

| 레이어 | 목표 커버리지 | 비고 |
|---|---|---|
| Util | 90% 이상 | DBManager, IdGenerator, DateTimeUtil |
| Model | 100% | 순수 비즈니스 로직, 단순 |
| Repository | 85% 이상 | CRUD + 추가 조회 메서드 |
| Controller | 90% 이상 | 핵심 로직, 분기 모두 커버 |
| View | 수동 테스트 | 콘솔 I/O 특성상 제외 |
