# Class Design: OrderController

## 개요

주문(Order) 관련 비즈니스 로직 처리 담당.  
주문 접수, 승인, 거절 로직 및 재고 판단과 생산 라인 연동을 처리한다.

## 클래스 정의

```cpp
class OrderController {
public:
    OrderController(OrderRepository& orderRepo,
                    SampleRepository& sampleRepo,
                    ProductionQueueRepository& productionRepo);

    // 주문 접수
    std::optional<Order> placeOrder(const std::string& sampleId,
                                    const std::string& customerName,
                                    int quantity);

    // 주문 승인/거절
    bool approveOrder(const std::string& orderId);  // 재고 확인 후 자동 분기
    bool rejectOrder(const std::string& orderId);

    // 주문 조회
    std::vector<Order> getReservedOrders();
    std::vector<Order> getOrdersByStatus(OrderStatus status);
    std::optional<Order> getOrderById(const std::string& orderId);

    // 모니터링용
    std::map<OrderStatus, int> getOrderCountByStatus();

private:
    OrderRepository& orderRepo_;
    SampleRepository& sampleRepo_;
    ProductionQueueRepository& productionRepo_;

    std::string generateOrderId();   // ORD-YYYYMMDD-NNNN 생성
    bool processWithStock(const Order& order, const Sample& sample);    // 재고 충분 처리
    bool processWithProduction(const Order& order, const Sample& sample); // 재고 부족 처리
};
```

## 주요 메서드 동작

### placeOrder
```
1. sampleId 존재 여부 확인 → 없으면 nullopt
2. quantity > 0 확인
3. orderId 생성 (ORD-YYYYMMDD-NNNN)
4. 상태 RESERVED로 Order 저장
5. 생성된 Order 반환
```

### approveOrder
```
1. orderId로 주문 조회 → 없거나 RESERVED 아니면 false
2. 해당 시료의 현재 재고 조회
3. 재고 >= 주문수량
   → processWithStock: 재고 차감, 상태 CONFIRMED
4. 재고 < 주문수량
   → processWithProduction: 생산 큐 등록, 상태 PRODUCING
```

### processWithProduction
```
1. 부족분 계산: shortfall = quantity - stock
2. 실 생산량: actualQty = ceil(shortfall / (yieldRate * 0.9))
3. 총 생산시간: totalTime = avgProductionTime * actualQty
4. ProductionJob 생성 후 큐에 등록
5. 주문 상태 → PRODUCING
```
