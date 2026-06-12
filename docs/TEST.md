# TEST.md — 테스트 계획 및 현황

**프로젝트**: 반도체 시료 생산주문관리 시스템 (S-Semi SampleOrderSystem)  
**최종 수정**: 2026-06-12  
**프레임워크**: Google Test (gtest v1.14.0)  
**원칙**: TDD — Red → Green → Refactor

---

## 현재 테스트 현황

| 테스트 스위트 | 파일 | 테스트 수 | 상태 |
|---|---|---|---|
| UtilLayer | test_db_manager, test_datetime_util, test_id_generator, test_console_util, test_dummy_data_generator | 23 | PASS |
| ModelLayer | test_sample, test_order, test_production_job | 15 | PASS |
| RepositoryLayer | test_sample_repository, test_order_repository, test_production_queue_repository | 21 | PASS |
| ControllerLayer | test_order_controller, test_production_controller | 9 | PASS |
| **합계** | **14개 파일** | **72** | **전체 PASS** |
| IntegrationTests | *(미구현 → 이 문서에서 계획)* | — | 계획 중 |

---

## 테스트 파일 구조

```
tests/
├── util/
│   ├── test_dummy.cpp                      # 플레이스홀더
│   ├── test_db_manager.cpp                 # DBManager CRUD + resetAllData
│   ├── test_datetime_util.cpp              # ISO8601 포맷 검증
│   ├── test_id_generator.cpp               # ID 형식 및 패딩 검증
│   ├── test_console_util.cpp               # readValidInt 범위 검증
│   └── test_dummy_data_generator.cpp       # 더미 데이터 삽입 검증
├── model/
│   ├── test_sample.cpp                     # hasEnoughStock, getShortfall, getStockStatus
│   ├── test_order.cpp                      # OrderStatus 전이
│   └── test_production_job.cpp             # 값 객체 getter
├── repository/
│   ├── test_sample_repository.cpp          # SQLite CRUD
│   ├── test_order_repository.cpp           # 상태별 조회, 카운트
│   └── test_production_queue_repository.cpp # FIFO enqueue/dequeue
├── controller/
│   ├── test_order_controller.cpp           # 승인/거절/재고 분기
│   └── test_production_controller.cpp      # 생산량 공식 검증
└── integration/                            # ← 신규 추가 예정
    ├── test_it_stock_sufficient_flow.cpp   # IT-01
    ├── test_it_production_flow.cpp         # IT-02
    ├── test_it_reject_flow.cpp             # IT-03
    ├── test_it_multi_order_flow.cpp        # IT-04
    └── test_it_fifo_production.cpp         # IT-05
```

---

## 단위 테스트 vs 통합 테스트 구분

```
단위 테스트 (현재 구현됨)
  → 각 레이어를 독립적으로 검증
  → OrderController 만 테스트 → approveOrder() 자체는 맞음
  → ReleaseController 만 테스트 → releaseOrder() 자체는 맞음
  → 두 Controller가 함께 동작할 때의 재고 흐름은 검증 불가 ❌

통합 테스트 (이 문서에서 계획)
  → Controller + Repository + Model 전 레이어를 실제 DB로 연결
  → 실제 주문 흐름(접수→승인→출고) 전체를 하나의 테스트로 검증
  → 레이어 경계를 넘나드는 버그(재고 이중 차감 등)를 잡을 수 있음 ✅
```

> **실증 사례**: 이번 개발 중 `approveOrder()`와 `releaseOrder()`가 각각 재고를 차감하는
> 이중 차감 버그가 존재했다. 단위 테스트는 이를 잡지 못했고, 코드 리뷰에서 발견됐다.
> 통합 테스트 IT-01이 있었다면 즉시 발견할 수 있었던 버그다.

---

## 통합 테스트 공통 설정

모든 통합 테스트는 다음 Fixture를 공유한다.

```cpp
class IntegrationTestBase : public ::testing::Test {
protected:
    void SetUp() override {
        DBManager::getInstance().initialize(":memory:");
        sqlite3* db = DBManager::getInstance().getDB();

        sampleRepo   = std::make_unique<SampleRepository>(db);
        orderRepo    = std::make_unique<OrderRepository>(db);
        prodQueueRepo = std::make_unique<ProductionQueueRepository>(db);

        sampleCtrl  = std::make_unique<SampleController>(sampleRepo.get());
        orderCtrl   = std::make_unique<OrderController>(
                          sampleRepo.get(), orderRepo.get(), prodQueueRepo.get());
        releaseCtrl = std::make_unique<ReleaseController>(
                          orderRepo.get(), sampleRepo.get(), prodQueueRepo.get());
    }
    void TearDown() override {
        releaseCtrl.reset(); orderCtrl.reset(); sampleCtrl.reset();
        sampleRepo.reset(); orderRepo.reset(); prodQueueRepo.reset();
        DBManager::getInstance().close();
    }

    // 편의 메서드: 시료 등록 후 ID 반환
    std::string registerSample(const std::string& name,
                                double avgTime, double yieldRate, int stock) {
        return sampleCtrl->registerSample(name, avgTime, yieldRate, stock);
    }

    std::unique_ptr<SampleRepository>         sampleRepo;
    std::unique_ptr<OrderRepository>          orderRepo;
    std::unique_ptr<ProductionQueueRepository> prodQueueRepo;
    std::unique_ptr<SampleController>         sampleCtrl;
    std::unique_ptr<OrderController>          orderCtrl;
    std::unique_ptr<ReleaseController>        releaseCtrl;
};
```

---

## IT-01: 재고 충분 완전 흐름

**목적**: 가장 기본적인 흐름인 `RESERVED → CONFIRMED → RELEASED`에서
재고가 **출고 시 단 1회만 차감**되는지 검증한다.

**왜 중요한가**: `approveOrder()`와 `releaseOrder()`가 각각 재고를 건드릴 경우
이중 차감 버그가 발생한다. 이 테스트가 단위 테스트로는 잡을 수 없는 그 버그를 잡는다.

### 사용자 입력 시퀀스

```
[메인 메뉴] 1 → 시료 관리
  [시료 메뉴] 1 → 시료 등록
    Name       : AlphaX
    AvgTime    : 10.0
    YieldRate  : 0.92
    InitStock  : 200
  → 등록 완료 (S-001)
  [시료 메뉴] 0 → 뒤로

[메인 메뉴] 2 → 시료 주문
  SampleID     : S-001
  CustomerName : CorpA
  Quantity     : 100
  Confirm? [Y] → Y
  → 주문 접수 완료 (ORD-20260612-0001)

[메인 메뉴] 3 → 주문 승인/거절
  [목록 확인] ORD-20260612-0001 / S-001 / CorpA / 100
  OrderID   : ORD-20260612-0001
  Action    : 1 (승인)
  → APPROVED (재고 충분 → CONFIRMED)

[메인 메뉴] 4 → 모니터링
  → CONFIRMED: 1, RESERVED: 0
  → S-001 Stock: 200, Pending: 0 → Status: OK

[메인 메뉴] 6 → 출고 처리
  OrderID   : ORD-20260612-0001
  → RELEASED

[메인 메뉴] 4 → 모니터링 (최종 확인)
  → RELEASED: 1
  → S-001 Stock: 100 (200 - 100)
```

### 테스트 케이스

```cpp
// IT-01-A: 승인 후 재고 불변 (차감은 출고 시)
TEST_F(IntegrationTest_StockSufficientFlow, Approve_DoesNotDeductStock) {
    std::string sid = registerSample("AlphaX", 10.0, 0.92, 200);
    std::string oid = orderCtrl->placeOrder(sid, "CorpA", 100);

    orderCtrl->approveOrder(oid);

    auto sample = sampleRepo->findById(sid);
    EXPECT_EQ(200, sample->getStock());  // 승인 후 재고 그대로
    EXPECT_EQ(OrderStatus::CONFIRMED, orderRepo->findById(oid)->getStatus());
}

// IT-01-B: 출고 후 재고가 정확히 1회 차감됨
TEST_F(IntegrationTest_StockSufficientFlow, Release_DeductsStockExactlyOnce) {
    std::string sid = registerSample("AlphaX", 10.0, 0.92, 200);
    std::string oid = orderCtrl->placeOrder(sid, "CorpA", 100);

    orderCtrl->approveOrder(oid);
    releaseCtrl->releaseOrder(oid);

    auto sample = sampleRepo->findById(sid);
    EXPECT_EQ(100, sample->getStock());  // 200 - 100 = 100
    EXPECT_EQ(OrderStatus::RELEASED, orderRepo->findById(oid)->getStatus());
}

// IT-01-C: 전체 상태 전이 순서 검증
TEST_F(IntegrationTest_StockSufficientFlow, FullFlow_StatusTransitions) {
    std::string sid = registerSample("AlphaX", 10.0, 0.92, 200);
    std::string oid = orderCtrl->placeOrder(sid, "CorpA", 100);
    EXPECT_EQ(OrderStatus::RESERVED,  orderRepo->findById(oid)->getStatus());

    orderCtrl->approveOrder(oid);
    EXPECT_EQ(OrderStatus::CONFIRMED, orderRepo->findById(oid)->getStatus());

    releaseCtrl->releaseOrder(oid);
    EXPECT_EQ(OrderStatus::RELEASED,  orderRepo->findById(oid)->getStatus());
}
```

### 기대 결과

| 시점 | 재고 | 주문 상태 |
|---|---|---|
| 시료 등록 후 | 200 | — |
| 주문 접수 후 | 200 | RESERVED |
| 승인 후 | 200 | CONFIRMED |
| 출고 후 | 100 | RELEASED |

---

## IT-02: 재고 부족 → 생산 → 출고 흐름

**목적**: 재고 부족 시 `RESERVED → PRODUCING → CONFIRMED → RELEASED` 전체 경로를
검증하고, 생산량 공식 `ceil(부족분 / (수율 × 0.9))`와 재고 증감이 정확한지 확인한다.

### 사용자 입력 시퀀스

```
[메인 메뉴] 1 → 시료 관리
  [시료 메뉴] 1 → 시료 등록
    Name       : GammaZ
    AvgTime    : 8.0
    YieldRate  : 0.92
    InitStock  : 30
  → 등록 완료 (S-001)

[메인 메뉴] 2 → 시료 주문
  SampleID     : S-001
  CustomerName : CorpB
  Quantity     : 200
  Confirm? [Y] → Y
  → 주문 접수 완료 (ORD-20260612-0001)

[메인 메뉴] 3 → 주문 승인/거절
  OrderID   : ORD-20260612-0001
  Action    : 1 (승인)
  → APPROVED (재고 부족 → PRODUCING, 생산 큐 등록)
  → 부족분 = 200 - 30 = 170
  → 실 생산량 = ceil(170 / (0.92 × 0.9)) = ceil(205.3) = 206

[메인 메뉴] 5 → 생산라인 조회
  [목록 확인] QueueID:1 / ORD-20260612-0001 / Qty:206 / Time:1648.0
  OrderID   : ORD-20260612-0001
  → 생산 완료 처리
  → 재고: 30 + 206 = 236, 주문 상태: CONFIRMED

[메인 메뉴] 6 → 출고 처리
  OrderID   : ORD-20260612-0001
  → RELEASED
  → 재고: 236 - 200 = 36
```

### 테스트 케이스

```cpp
// IT-02-A: 재고 부족 시 PRODUCING + 생산 큐 등록
TEST_F(IntegrationTest_ProductionFlow, Approve_InsufficientStock_BecomesProducing) {
    std::string sid = registerSample("GammaZ", 8.0, 0.92, 30);
    std::string oid = orderCtrl->placeOrder(sid, "CorpB", 200);

    orderCtrl->approveOrder(oid);

    EXPECT_EQ(OrderStatus::PRODUCING, orderRepo->findById(oid)->getStatus());
    EXPECT_FALSE(prodQueueRepo->isEmpty());
}

// IT-02-B: 생산량 공식 검증 (부족분=170, 수율=0.92 → 206)
TEST_F(IntegrationTest_ProductionFlow, ProductionQty_Formula_IsCorrect) {
    std::string sid = registerSample("GammaZ", 8.0, 0.92, 30);
    std::string oid = orderCtrl->placeOrder(sid, "CorpB", 200);
    orderCtrl->approveOrder(oid);

    auto job = prodQueueRepo->peek();
    ASSERT_TRUE(job.has_value());
    EXPECT_EQ(206, job->getActualProductionQty());  // ceil(170/(0.92*0.9))
}

// IT-02-C: 생산 완료 후 재고 증가 + CONFIRMED 전이
TEST_F(IntegrationTest_ProductionFlow, CompleteProduction_IncreasesStockAndConfirms) {
    std::string sid = registerSample("GammaZ", 8.0, 0.92, 30);
    std::string oid = orderCtrl->placeOrder(sid, "CorpB", 200);
    orderCtrl->approveOrder(oid);

    auto job = prodQueueRepo->peek();
    prodQueueRepo->remove(std::to_string(job->getQueueId()));
    releaseCtrl->completeProduction(oid, job->getActualProductionQty());

    EXPECT_EQ(236, sampleRepo->findById(sid)->getStock());  // 30 + 206
    EXPECT_EQ(OrderStatus::CONFIRMED, orderRepo->findById(oid)->getStatus());
}

// IT-02-D: 출고 후 최종 재고 검증
TEST_F(IntegrationTest_ProductionFlow, FullProductionFlow_FinalStockIsCorrect) {
    std::string sid = registerSample("GammaZ", 8.0, 0.92, 30);
    std::string oid = orderCtrl->placeOrder(sid, "CorpB", 200);
    orderCtrl->approveOrder(oid);

    auto job = prodQueueRepo->peek();
    prodQueueRepo->remove(std::to_string(job->getQueueId()));
    releaseCtrl->completeProduction(oid, job->getActualProductionQty());
    releaseCtrl->releaseOrder(oid);

    EXPECT_EQ(36, sampleRepo->findById(sid)->getStock());   // 236 - 200
    EXPECT_EQ(OrderStatus::RELEASED, orderRepo->findById(oid)->getStatus());
}
```

### 기대 결과

| 시점 | 재고 | 주문 상태 | 생산 큐 |
|---|---|---|---|
| 시료 등록 후 | 30 | — | 비어있음 |
| 주문 접수 후 | 30 | RESERVED | 비어있음 |
| 승인 후 | 30 | PRODUCING | 1건 (qty=206) |
| 생산 완료 후 | 236 | CONFIRMED | 비어있음 |
| 출고 후 | 36 | RELEASED | 비어있음 |

---

## IT-03: 주문 거절 흐름

**목적**: 거절 시 재고에 변화가 없고 모니터링 집계에서 제외되는지 검증한다.

### 사용자 입력 시퀀스

```
[메인 메뉴] 1 → 시료 관리 → 시료 등록
  Name: BetaY / AvgTime: 15.0 / Yield: 0.90 / Stock: 100

[메인 메뉴] 2 → 시료 주문
  SampleID: S-001 / Customer: CorpC / Qty: 50 / Y

[메인 메뉴] 3 → 주문 승인/거절
  OrderID : ORD-20260612-0001
  Action  : 2 (거절)
  → REJECTED

[메인 메뉴] 4 → 모니터링
  → REJECTED 건수: 표시 안됨
  → S-001 Stock: 100 (변화 없음)
```

### 테스트 케이스

```cpp
// IT-03-A: 거절 후 재고 불변
TEST_F(IntegrationTest_RejectFlow, Reject_DoesNotChangeStock) {
    std::string sid = registerSample("BetaY", 15.0, 0.90, 100);
    std::string oid = orderCtrl->placeOrder(sid, "CorpC", 50);

    orderCtrl->rejectOrder(oid);

    EXPECT_EQ(100, sampleRepo->findById(sid)->getStock());
    EXPECT_EQ(OrderStatus::REJECTED, orderRepo->findById(oid)->getStatus());
}

// IT-03-B: 거절 주문은 생산 큐에 등록되지 않음
TEST_F(IntegrationTest_RejectFlow, Reject_DoesNotEnqueueProductionJob) {
    std::string sid = registerSample("BetaY", 15.0, 0.90, 10);  // 재고 부족
    std::string oid = orderCtrl->placeOrder(sid, "CorpC", 100);

    orderCtrl->rejectOrder(oid);

    EXPECT_TRUE(prodQueueRepo->isEmpty());
}

// IT-03-C: 거절된 주문은 CONFIRMED 목록에 미포함
TEST_F(IntegrationTest_RejectFlow, Reject_NotInConfirmedList) {
    std::string sid = registerSample("BetaY", 15.0, 0.90, 100);
    std::string oid = orderCtrl->placeOrder(sid, "CorpC", 50);

    orderCtrl->rejectOrder(oid);

    auto confirmed = orderRepo->findByStatus(OrderStatus::CONFIRMED);
    EXPECT_TRUE(confirmed.empty());
}
```

---

## IT-04: 동일 시료 복수 주문 순차 처리

**목적**: 같은 시료에 여러 주문이 들어올 때 각 출고 후 재고가 누적 차감되어
정확히 계산되는지 검증한다.

### 사용자 입력 시퀀스

```
[시료 등록] S-001 / AlphaX / Stock: 300

[주문 1 접수] S-001 / CorpA / Qty: 80
[주문 2 접수] S-001 / CorpB / Qty: 120

[주문 1 승인] → CONFIRMED (재고 300 → 변화 없음)
[주문 2 승인] → CONFIRMED (재고 여전히 300)

[주문 1 출고] → RELEASED, Stock: 300 - 80 = 220
[주문 2 출고] → RELEASED, Stock: 220 - 120 = 100
```

### 테스트 케이스

```cpp
// IT-04-A: 복수 승인 후 재고 불변
TEST_F(IntegrationTest_MultiOrderFlow, MultipleApprovals_StockUnchanged) {
    std::string sid = registerSample("AlphaX", 10.0, 0.92, 300);
    std::string oid1 = orderCtrl->placeOrder(sid, "CorpA", 80);
    std::string oid2 = orderCtrl->placeOrder(sid, "CorpB", 120);

    orderCtrl->approveOrder(oid1);
    orderCtrl->approveOrder(oid2);

    EXPECT_EQ(300, sampleRepo->findById(sid)->getStock());
}

// IT-04-B: 순차 출고 후 재고 누적 차감
TEST_F(IntegrationTest_MultiOrderFlow, SequentialRelease_CumulativeStockDeduction) {
    std::string sid = registerSample("AlphaX", 10.0, 0.92, 300);
    std::string oid1 = orderCtrl->placeOrder(sid, "CorpA", 80);
    std::string oid2 = orderCtrl->placeOrder(sid, "CorpB", 120);
    orderCtrl->approveOrder(oid1);
    orderCtrl->approveOrder(oid2);

    releaseCtrl->releaseOrder(oid1);
    EXPECT_EQ(220, sampleRepo->findById(sid)->getStock());  // 300-80

    releaseCtrl->releaseOrder(oid2);
    EXPECT_EQ(100, sampleRepo->findById(sid)->getStock());  // 220-120
}
```

---

## IT-05: FIFO 생산 큐 순서 검증

**목적**: 재고 부족 주문이 복수일 때 생산 큐가 FIFO 순서로 처리되는지 검증한다.

### 사용자 입력 시퀀스

```
[시료 등록] S-001 / DeltaW / Stock: 0 (재고 없음)

[주문 1 접수] S-001 / CorpD / Qty: 50  → 승인 → PRODUCING (큐 등록 #1)
[주문 2 접수] S-001 / CorpE / Qty: 80  → 승인 → PRODUCING (큐 등록 #2)

[생산 라인]
  큐 목록: [#1 ORD-001, #2 ORD-002]
  먼저 처리: ORD-001 (선입선출)
  → ORD-001 CONFIRMED, ORD-002 여전히 PRODUCING
```

### 테스트 케이스

```cpp
// IT-05-A: 먼저 들어온 주문이 큐 앞에 위치
TEST_F(IntegrationTest_FifoProduction, FIFO_FirstEnqueuedIsFirstInQueue) {
    std::string sid = registerSample("DeltaW", 12.0, 0.90, 0);
    std::string oid1 = orderCtrl->placeOrder(sid, "CorpD", 50);
    std::string oid2 = orderCtrl->placeOrder(sid, "CorpE", 80);

    orderCtrl->approveOrder(oid1);
    orderCtrl->approveOrder(oid2);

    auto firstJob = prodQueueRepo->peek();
    ASSERT_TRUE(firstJob.has_value());
    EXPECT_EQ(oid1, firstJob->getOrderId());  // oid1이 먼저 들어왔으므로 먼저 나옴
}

// IT-05-B: 첫 번째 생산 완료 후 두 번째는 여전히 PRODUCING
TEST_F(IntegrationTest_FifoProduction, FIFO_SecondOrderRemainsProducingAfterFirst) {
    std::string sid = registerSample("DeltaW", 12.0, 0.90, 0);
    std::string oid1 = orderCtrl->placeOrder(sid, "CorpD", 50);
    std::string oid2 = orderCtrl->placeOrder(sid, "CorpE", 80);
    orderCtrl->approveOrder(oid1);
    orderCtrl->approveOrder(oid2);

    auto job = prodQueueRepo->peek();
    prodQueueRepo->remove(std::to_string(job->getQueueId()));
    releaseCtrl->completeProduction(oid1, job->getActualProductionQty());

    EXPECT_EQ(OrderStatus::CONFIRMED, orderRepo->findById(oid1)->getStatus());
    EXPECT_EQ(OrderStatus::PRODUCING, orderRepo->findById(oid2)->getStatus());
    EXPECT_FALSE(prodQueueRepo->isEmpty());  // oid2 아직 큐에 있음
}
```

---

## 수동 테스트 시나리오 (콘솔 직접 실행)

단위/통합 테스트가 커버하지 못하는 **View 레이어 + 사용자 입력 흐름**을 수동으로 검증한다.

### 수동 테스트 준비

```batch
"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
cd C:\Reviewer\Day03-Project\SampleOrderSystem
cmake -S . -B build -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Debug
cd build && nmake
cd ..
build\SampleOrderSystem.exe
```

---

### MT-01: 입력 검증 — 범위 초과

**목적**: 잘못된 메뉴 번호, 음수/0 수량, 범위 밖 수율 입력 시 재입력 요청 확인

```
[메인 메뉴] 입력: 9
→ "Invalid choice. Enter 0~6: " 메시지 출력 후 재입력 대기

[메인 메뉴] 입력: -1
→ "Invalid choice. Enter 0~6: " 메시지 출력

[메인 메뉴] 입력: abc
→ "Invalid choice. Enter 0~6: " 메시지 출력

[메인 메뉴] 1 → 시료 관리 → 1 → 시료 등록
  AvgTime: -5.0
  → "Invalid value. Must be greater than 0." 재입력 대기

  YieldRate: 1.5
  → "Invalid value. Must be between 0 (exclusive) and 1 (inclusive)." 재입력 대기

  Stock: -10
  → "Invalid value. Must be 0 or greater." 재입력 대기

[메인 메뉴] 2 → 시료 주문
  Quantity: 0
  → "Invalid value. Quantity must be greater than 0." 재입력 대기
```

**판정 기준**: 잘못된 입력마다 안내 문구가 출력되고, 프로그램이 종료되지 않으며, 재입력 후 정상 동작한다.

---

### MT-02: 화면 전환 — Enter 후 화면 클리어

**목적**: 각 메뉴 동작 후 "Press Enter to continue..." 메시지 출력 및 Enter 입력 시 화면이 지워지는지 확인

```
[어떤 메뉴든 동작 완료 후]
→ "Press Enter to continue..." 출력 확인
→ Enter 입력
→ 화면 지워지고 메인 메뉴 재출력 확인
```

---

### MT-03: DB 영속성 — 재실행 후 데이터 유지

**목적**: 프로그램 종료 후 재실행 시 이전 데이터가 그대로 유지되는지 검증

```
[1회 실행]
  DUMMY 입력 → 시료 3개, 주문 6개 삽입
  메인 메뉴 상단 Summary: Samples:3, Orders:6 확인
  0 입력 → 종료

[2회 실행]
  메인 메뉴 상단 Summary: Samples:3, Orders:6 확인 (데이터 유지)
```

---

### MT-04: RESET 후 데이터 초기화 확인

```
[메인 메뉴] DUMMY 입력 → 더미 데이터 삽입 확인
[메인 메뉴] RESET 입력
  → "[WARNING] This will permanently delete ALL..."
  입력: CONFIRM
  → "All data has been reset."
[메인 메뉴] 4 → 모니터링
  → 모든 카운트 0
  → 시료 목록 없음
```

---

### MT-05: DUMMY → 전체 흐름 연속 테스트

**목적**: 더미 데이터를 삽입하고 전체 기능을 연속으로 검증하는 통합 실행 시나리오

```
[단계 1] RESET → CONFIRM (초기화)
[단계 2] DUMMY (더미 데이터 삽입: 시료 3, 주문 6)
[단계 3] 3 → 주문 승인/거절
  RESERVED 목록 확인 (6건)
  첫 번째 주문 → 승인 (재고 상황에 따라 CONFIRMED 또는 PRODUCING)
[단계 4] 필요 시 5 → 생산 큐에서 생산 완료 처리
[단계 5] 6 → 출고 처리
[단계 6] 4 → 모니터링으로 최종 상태 확인
```

---

## 통합 테스트 구현 계획

| ID | 파일 | 테스트 수 | 우선순위 |
|---|---|---|---|
| IT-01 | test_it_stock_sufficient_flow.cpp | 3 | **높음** (이중차감 방지) |
| IT-02 | test_it_production_flow.cpp | 4 | **높음** (생산 공식 + 전 흐름) |
| IT-03 | test_it_reject_flow.cpp | 3 | 중간 |
| IT-04 | test_it_multi_order_flow.cpp | 2 | 중간 |
| IT-05 | test_it_fifo_production.cpp | 2 | 중간 |
| **합계** | **5개 파일** | **14** | — |

구현 후 총 테스트: **72 + 14 = 86개**

---

## 테스트 실행 방법

### 전체 테스트 실행

```batch
"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
cd C:\Reviewer\Day03-Project\SampleOrderSystem
cmake -S . -B build -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Debug
cd build && nmake
cd ..
build\run_tests.exe
```

### 특정 테스트만 실행

```batch
rem 통합 테스트만
build\run_tests.exe --gtest_filter="Integration*"

rem 특정 시나리오만
build\run_tests.exe --gtest_filter="IntegrationTest_ProductionFlow*"

rem 상세 출력
build\run_tests.exe --gtest_verbose
```

---

## 테스트 커버리지 목표

| 레이어 | 단위 테스트 | 통합 테스트 | 수동 테스트 |
|---|---|---|---|
| Util | 23개 ✅ | — | MT-03, MT-04 |
| Model | 15개 ✅ | — | — |
| Repository | 21개 ✅ | — | — |
| Controller | 9개 ✅ | IT-01 ~ IT-05 | — |
| View | 제외 | — | MT-01 ~ MT-05 |
| **흐름 전체** | — | **14개 예정** | — |

> Controller 테스트는 단일 레이어만 검증하므로 레이어 간 상호작용은
> 통합 테스트(IT-01~05)와 수동 테스트(MT-01~05)가 담당한다.
