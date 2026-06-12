# Class Design: ProductionController

## 개요

생산 라인 관련 비즈니스 로직 처리 담당.  
FIFO 큐 기반의 생산 현황 조회 및 생산 완료 처리를 담당한다.

## 클래스 정의

```cpp
class ProductionController {
public:
    ProductionController(ProductionQueueRepository& productionRepo,
                         OrderRepository& orderRepo,
                         SampleRepository& sampleRepo);

    // 생산 현황 조회
    std::optional<ProductionJob> getCurrentJob();    // 현재 생산 중인 작업 (큐 맨 앞)
    std::vector<ProductionJob> getWaitingJobs();     // 대기 중인 작업 목록 (현재 작업 제외)
    std::vector<ProductionJob> getAllJobs();          // 전체 큐 (현재 + 대기)

    // 생산 완료 처리
    bool completeCurrentJob();   // 현재 작업 완료: 재고 증가, PRODUCING → CONFIRMED, 큐에서 제거

    // 생산량 계산 (정적 유틸)
    static int calcActualProductionQty(int shortfall, double yieldRate);
    static double calcTotalProductionTime(double avgTime, int actualQty);

private:
    ProductionQueueRepository& productionRepo_;
    OrderRepository& orderRepo_;
    SampleRepository& sampleRepo_;
};
```

## 주요 메서드 동작

### completeCurrentJob
```
1. 큐에서 next() 조회 → 없으면 false
2. orderId로 주문 조회
3. sampleId로 시료 조회
4. 시료 재고 += actualProductionQty
5. 주문 상태 PRODUCING → CONFIRMED
6. 생산 큐에서 해당 job 제거
```

### calcActualProductionQty (정적)
```
return ceil(shortfall / (yieldRate * 0.9))
```

### calcTotalProductionTime (정적)
```
return avgTime * actualQty
```

## 설계 주안점

- `calcActualProductionQty`, `calcTotalProductionTime`은 `static`으로 선언
  → 단위 테스트에서 인스턴스 없이 직접 호출 가능
  → TDD 작성이 용이
