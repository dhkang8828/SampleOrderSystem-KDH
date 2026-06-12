# Class Design: ReleaseController

## 개요

출고 처리 비즈니스 로직 담당.  
CONFIRMED 상태 주문을 RELEASED로 전환한다.

## 클래스 정의

```cpp
class ReleaseController {
public:
    explicit ReleaseController(OrderRepository& orderRepo);

    // 출고 가능 주문 조회
    std::vector<Order> getConfirmedOrders();

    // 출고 처리
    bool releaseOrder(const std::string& orderId);

private:
    OrderRepository& orderRepo_;
};
```

## 주요 메서드 동작

### releaseOrder
```
1. orderId로 주문 조회 → 없으면 false
2. 상태가 CONFIRMED인지 확인 → 아니면 false
3. 주문 상태 CONFIRMED → RELEASED
4. true 반환
```
