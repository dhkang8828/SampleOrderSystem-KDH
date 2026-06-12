# Class Design: Controller Layer

## 포함 클래스
- SampleController
- OrderController
- ProductionController
- ReleaseController

---

## 1. SampleController

### 개요
시료(Sample) 관련 비즈니스 로직 처리 담당.

### 클래스 정의
```cpp
class SampleController {
public:
    explicit SampleController(SampleRepository& sampleRepo);

    bool registerSample(const std::string& sampleId,
                        const std::string& name,
                        double avgProductionTime,
                        double yieldRate,
                        int initialStock);

    std::vector<Sample> getAllSamples();
    std::optional<Sample> getSampleById(const std::string& sampleId);
    std::vector<Sample> searchSamplesByName(const std::string& keyword);

    bool updateStock(const std::string& sampleId, int newStock);
    bool increaseStock(const std::string& sampleId, int amount);
    bool decreaseStock(const std::string& sampleId, int amount);
    bool existsSample(const std::string& sampleId);

private:
    SampleRepository& sampleRepo_;
    bool isValidYieldRate(double yieldRate) const;      // 0.0 < yieldRate <= 1.0
    bool isValidProductionTime(double time) const;      // time > 0
};
```

### 주요 메서드 동작
```
registerSample:
  1. sampleId 중복 체크 → 중복이면 false
  2. 입력값 유효성 검사 (수율 범위, 생산시간 양수)
  3. Sample 객체 생성 후 Repository 저장

decreaseStock:
  1. 현재 재고 조회
  2. 재고 >= amount 확인
  3. newStock = 현재재고 - amount 후 업데이트
```

---

## 2. OrderController

### 개요
주문 접수, 승인, 거절 로직 및 재고 판단과 생산 라인 연동 처리 담당.

### 클래스 정의
```cpp
class OrderController {
public:
    OrderController(OrderRepository& orderRepo,
                    SampleRepository& sampleRepo,
                    ProductionQueueRepository& productionRepo);

    std::optional<Order> placeOrder(const std::string& sampleId,
                                    const std::string& customerName,
                                    int quantity);

    bool approveOrder(const std::string& orderId);
    bool rejectOrder(const std::string& orderId);

    std::vector<Order> getReservedOrders();
    std::vector<Order> getOrdersByStatus(OrderStatus status);
    std::optional<Order> getOrderById(const std::string& orderId);
    std::map<OrderStatus, int> getOrderCountByStatus();

private:
    OrderRepository& orderRepo_;
    SampleRepository& sampleRepo_;
    ProductionQueueRepository& productionRepo_;

    std::string generateOrderId();
    bool processWithStock(const Order& order, const Sample& sample);
    bool processWithProduction(const Order& order, const Sample& sample);
};
```

### 주요 메서드 동작
```
placeOrder:
  1. sampleId 존재 확인 → 없으면 nullopt
  2. quantity > 0 확인
  3. orderId 생성 (ORD-YYYYMMDD-NNNN)
  4. 상태 RESERVED로 저장 후 반환

approveOrder:
  1. 주문 조회 → RESERVED 상태 확인
  2. 시료 재고 조회
  3. 재고 >= 주문수량 → processWithStock (재고 차감, CONFIRMED)
  4. 재고 < 주문수량  → processWithProduction (생산 큐 등록, PRODUCING)

processWithProduction:
  1. 부족분 = quantity - stock
  2. 실 생산량 = ceil(부족분 / (yieldRate * 0.9))
  3. 총 생산시간 = avgProductionTime * 실생산량
  4. ProductionJob 생성 후 큐 등록
  5. 주문 상태 → PRODUCING
```

---

## 3. ProductionController

### 개요
생산 라인 현황 조회 및 생산 완료 처리 담당.

### 클래스 정의
```cpp
class ProductionController {
public:
    ProductionController(ProductionQueueRepository& productionRepo,
                         OrderRepository& orderRepo,
                         SampleRepository& sampleRepo);

    std::optional<ProductionJob> getCurrentJob();
    std::vector<ProductionJob> getWaitingJobs();
    std::vector<ProductionJob> getAllJobs();

    bool completeCurrentJob();

    static int calcActualProductionQty(int shortfall, double yieldRate);
    static double calcTotalProductionTime(double avgTime, int actualQty);

private:
    ProductionQueueRepository& productionRepo_;
    OrderRepository& orderRepo_;
    SampleRepository& sampleRepo_;
};
```

### 주요 메서드 동작
```
completeCurrentJob:
  1. 큐에서 next() 조회 → 없으면 false
  2. 주문 및 시료 조회
  3. 시료 재고 += actualProductionQty
  4. 주문 상태 PRODUCING → CONFIRMED
  5. 큐에서 job 제거

calcActualProductionQty (static):
  return ceil(shortfall / (yieldRate * 0.9))

calcTotalProductionTime (static):
  return avgTime * actualQty
```

> `static` 메서드로 선언하여 TDD 시 인스턴스 없이 직접 테스트 가능

---

## 4. ReleaseController

### 개요
CONFIRMED 상태 주문을 RELEASED로 전환하는 출고 처리 담당.

### 클래스 정의
```cpp
class ReleaseController {
public:
    explicit ReleaseController(OrderRepository& orderRepo);

    std::vector<Order> getConfirmedOrders();
    bool releaseOrder(const std::string& orderId);

private:
    OrderRepository& orderRepo_;
};
```

### 주요 메서드 동작
```
releaseOrder:
  1. orderId로 주문 조회 → 없으면 false
  2. 상태가 CONFIRMED 확인 → 아니면 false
  3. 주문 상태 CONFIRMED → RELEASED
  4. true 반환
```
