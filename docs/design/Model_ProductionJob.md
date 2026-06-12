# Class Design: ProductionJob

## 개요

생산 라인의 큐에 등록된 생산 작업 단위.  
Order가 재고 부족으로 승인될 때 생성되며, FIFO 큐로 관리된다.

## 클래스 정의

```cpp
class ProductionJob {
public:
    // Constructors
    ProductionJob();
    ProductionJob(int queueId,
                  const std::string& orderId,
                  int actualProductionQty,
                  double totalProductionTime,
                  const std::string& enqueuedAt);

    // Getters
    int getQueueId() const;
    std::string getOrderId() const;
    int getActualProductionQty() const;
    double getTotalProductionTime() const;
    std::string getEnqueuedAt() const;

    // Utility
    std::string toString() const;

private:
    int queueId_;                   // AUTO INCREMENT PK
    std::string orderId_;           // 연결된 주문번호
    int actualProductionQty_;       // 실 생산량 = ceil(부족분 / (수율 * 0.9))
    double totalProductionTime_;    // 총 생산시간 = 평균생산시간 * 실생산량 (min)
    std::string enqueuedAt_;        // 큐 등록 일시 (ISO 8601)
};
```

## 생산량 계산 공식

```
부족분        = 주문수량 - 현재재고
실 생산량     = ceil(부족분 / (수율 × 0.9))
총 생산 시간  = 평균 생산시간 × 실 생산량
```

### 예시
```
주문수량: 200 ea, 현재재고: 30 ea, 수율: 0.92, 평균생산시간: 0.8 min/ea

부족분       = 200 - 30 = 170 ea
실 생산량    = ceil(170 / (0.92 × 0.9)) = ceil(170 / 0.828) = ceil(205.3) = 206 ea
총 생산 시간 = 0.8 × 206 = 164.8 min
```

## 멤버 상세

| 멤버 | 타입 | 설명 | 제약 |
|---|---|---|---|
| queueId_ | int | 큐 순번 | AUTO INCREMENT, 1 이상 |
| orderId_ | string | 주문번호 (FK) | Order에 존재해야 함 |
| actualProductionQty_ | int | 실 생산량 | 1 이상 |
| totalProductionTime_ | double | 총 생산 시간 (min) | 0 초과 |
| enqueuedAt_ | string | 큐 등록 일시 | ISO 8601 형식 |
