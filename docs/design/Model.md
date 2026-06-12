# Class Design: Model Layer

## 포함 클래스
- Sample
- Order
- ProductionJob
- OrderStatus (enum)
- StockStatus (enum)

---

## 1. Sample

### 개요
반도체 시료(Sample)를 표현하는 도메인 모델. 시스템의 가장 기본 단위.
향후 시료 타입(웨이퍼, 에피택셜 등) 확장을 고려한 구조로 설계.

### 확장성 설계
```
Sample (base)
  └─ 향후 필요 시: WaferSample, EpitaxialSample 등 서브클래스 확장 가능
```

### 클래스 정의
```cpp
class Sample {
public:
    Sample();
    Sample(const std::string& sampleId,
           const std::string& name,
           double avgProductionTime,
           double yieldRate,
           int stock);

    std::string getSampleId() const;
    std::string getName() const;
    double getAvgProductionTime() const;
    double getYieldRate() const;
    int getStock() const;

    void setStock(int stock);

    bool hasEnoughStock(int requiredQty) const;
    int getShortfall(int requiredQty) const;          // 부족분, 음수면 0 반환
    StockStatus getStockStatus(int pendingQty) const; // 여유/부족/고갈 판단

    std::string toString() const;

private:
    std::string sampleId_;
    std::string name_;
    double avgProductionTime_;  // min/ea
    double yieldRate_;          // 0.0 ~ 1.0
    int stock_;
};
```

### StockStatus (enum)
```cpp
enum class StockStatus {
    SUFFICIENT,  // 여유: 주문 대비 재고 충분
    SHORTAGE,    // 부족: 주문 대비 재고 부족
    DEPLETED     // 고갈: 재고 0
};
```

### 멤버 상세
| 멤버 | 타입 | 설명 | 제약 |
|---|---|---|---|
| sampleId_ | string | 시료 고유 ID | 비어있으면 안 됨, 중복 불가 |
| name_ | string | 시료 이름 | 비어있으면 안 됨 |
| avgProductionTime_ | double | 평균 생산시간 (min/ea) | 0 초과 |
| yieldRate_ | double | 수율 | 0.0 초과 ~ 1.0 이하 |
| stock_ | int | 현재 재고 수량 | 0 이상 |

### 주요 메서드 동작
```
hasEnoughStock : stock_ >= requiredQty → true, 미만 → false
getShortfall   : requiredQty - stock_, 음수면 0
getStockStatus : stock_==0 → DEPLETED, stock_<pendingQty → SHORTAGE, 이상 → SUFFICIENT
```

---

## 2. Order

### 개요
고객의 시료 주문을 표현하는 도메인 모델.
OrderStatus를 통해 전체 주문 흐름을 추적한다.

### 클래스 정의
```cpp
class Order {
public:
    Order();
    Order(const std::string& orderId,
          const std::string& sampleId,
          const std::string& customerName,
          int quantity,
          OrderStatus status,
          const std::string& createdAt);

    std::string getOrderId() const;
    std::string getSampleId() const;
    std::string getCustomerName() const;
    int getQuantity() const;
    OrderStatus getStatus() const;
    std::string getCreatedAt() const;

    void setStatus(OrderStatus status);

    bool isReserved() const;
    bool isProducing() const;
    bool isConfirmed() const;
    bool isReleased() const;
    bool isRejected() const;

    std::string statusToString() const;
    std::string toString() const;

private:
    std::string orderId_;       // ORD-YYYYMMDD-NNNN
    std::string sampleId_;
    std::string customerName_;
    int quantity_;
    OrderStatus status_;
    std::string createdAt_;     // ISO 8601
};
```

### OrderStatus (enum)
```cpp
enum class OrderStatus {
    RESERVED,   // 주문 접수
    REJECTED,   // 주문 거절
    PRODUCING,  // 생산 중 (재고 부족)
    CONFIRMED,  // 출고 대기 중
    RELEASED    // 출고 완료
};
```

### 상태 전이 규칙
```
RESERVED  →  CONFIRMED  (승인 + 재고 충분)
RESERVED  →  PRODUCING  (승인 + 재고 부족)
RESERVED  →  REJECTED   (거절)
PRODUCING →  CONFIRMED  (생산 완료)
CONFIRMED →  RELEASED   (출고 처리)
```

### 멤버 상세
| 멤버 | 타입 | 설명 | 제약 |
|---|---|---|---|
| orderId_ | string | 주문번호 | ORD-YYYYMMDD-NNNN 형식 |
| sampleId_ | string | 시료 ID (FK) | Sample에 존재해야 함 |
| customerName_ | string | 고객명 | 비어있으면 안 됨 |
| quantity_ | int | 주문 수량 | 1 이상 |
| status_ | OrderStatus | 주문 상태 | 위 전이 규칙 준수 |
| createdAt_ | string | 주문 생성일시 | ISO 8601 형식 |

---

## 3. ProductionJob

### 개요
생산 라인의 큐에 등록된 생산 작업 단위.
Order가 재고 부족으로 승인될 때 생성되며, FIFO 큐로 관리된다.

### 클래스 정의
```cpp
class ProductionJob {
public:
    ProductionJob();
    ProductionJob(int queueId,
                  const std::string& orderId,
                  int actualProductionQty,
                  double totalProductionTime,
                  const std::string& enqueuedAt);

    int getQueueId() const;
    std::string getOrderId() const;
    int getActualProductionQty() const;
    double getTotalProductionTime() const;
    std::string getEnqueuedAt() const;

    std::string toString() const;

private:
    int queueId_;                   // AUTO INCREMENT PK
    std::string orderId_;
    int actualProductionQty_;       // ceil(부족분 / (수율 * 0.9))
    double totalProductionTime_;    // 평균생산시간 * 실생산량 (min)
    std::string enqueuedAt_;        // ISO 8601
};
```

### 생산량 계산 공식
```
부족분        = 주문수량 - 현재재고
실 생산량     = ceil(부족분 / (수율 × 0.9))
총 생산 시간  = 평균 생산시간 × 실 생산량

예시) 주문 200ea, 재고 30ea, 수율 0.92, 평균생산시간 0.8 min/ea
  부족분       = 170 ea
  실 생산량    = ceil(170 / (0.92 × 0.9)) = ceil(205.3) = 206 ea
  총 생산 시간 = 0.8 × 206 = 164.8 min
```

### 멤버 상세
| 멤버 | 타입 | 설명 | 제약 |
|---|---|---|---|
| queueId_ | int | 큐 순번 | AUTO INCREMENT |
| orderId_ | string | 주문번호 (FK) | Order에 존재해야 함 |
| actualProductionQty_ | int | 실 생산량 | 1 이상 |
| totalProductionTime_ | double | 총 생산 시간 (min) | 0 초과 |
| enqueuedAt_ | string | 큐 등록 일시 | ISO 8601 형식 |
