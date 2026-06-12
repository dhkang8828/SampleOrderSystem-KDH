# Class Design: Order

## 개요

고객의 시료 주문을 표현하는 도메인 모델.  
주문 상태(OrderStatus)를 통해 전체 주문 흐름을 추적한다.

## 클래스 정의

```cpp
class Order {
public:
    // Constructors
    Order();
    Order(const std::string& orderId,
          const std::string& sampleId,
          const std::string& customerName,
          int quantity,
          OrderStatus status,
          const std::string& createdAt);

    // Getters
    std::string getOrderId() const;
    std::string getSampleId() const;
    std::string getCustomerName() const;
    int getQuantity() const;
    OrderStatus getStatus() const;
    std::string getCreatedAt() const;

    // Setters
    void setStatus(OrderStatus status);

    // Business Logic
    bool isReserved() const;
    bool isProducing() const;
    bool isConfirmed() const;
    bool isReleased() const;
    bool isRejected() const;

    // Utility
    std::string statusToString() const;
    std::string toString() const;

private:
    std::string orderId_;       // ORD-YYYYMMDD-NNNN
    std::string sampleId_;
    std::string customerName_;
    int quantity_;
    OrderStatus status_;
    std::string createdAt_;     // ISO 8601 (예: 2026-06-12T09:32:15)
};
```

## OrderStatus (enum)

```cpp
enum class OrderStatus {
    RESERVED,   // 주문 접수
    REJECTED,   // 주문 거절
    PRODUCING,  // 생산 중 (재고 부족)
    CONFIRMED,  // 출고 대기 중
    RELEASED    // 출고 완료
};
```

## 상태 전이 규칙

```
RESERVED  →  CONFIRMED  (승인 + 재고 충분)
RESERVED  →  PRODUCING  (승인 + 재고 부족)
RESERVED  →  REJECTED   (거절)
PRODUCING →  CONFIRMED  (생산 완료)
CONFIRMED →  RELEASED   (출고 처리)
```

- 위 전이 외의 상태 변경은 허용하지 않음

## 멤버 상세

| 멤버 | 타입 | 설명 | 제약 |
|---|---|---|---|
| orderId_ | string | 주문번호 | ORD-YYYYMMDD-NNNN 형식 |
| sampleId_ | string | 시료 ID (FK) | Sample에 존재해야 함 |
| customerName_ | string | 고객명 | 비어있으면 안 됨 |
| quantity_ | int | 주문 수량 | 1 이상 |
| status_ | OrderStatus | 주문 상태 | 위 상태 전이 규칙 준수 |
| createdAt_ | string | 주문 생성일시 | ISO 8601 형식 |
