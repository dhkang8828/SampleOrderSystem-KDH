# Class Design: Sample

## 개요

반도체 시료(Sample)를 표현하는 도메인 모델. 시스템의 가장 기본 단위.  
향후 시료 타입(웨이퍼, 에피택셜 등) 확장을 고려한 구조로 설계.

## 확장성 설계

```
Sample (base)
  └─ 향후 필요 시: WaferSample, EpitaxialSample 등 서브클래스 확장 가능
```

- 핵심 속성은 `Sample`에 보유
- 타입별 특수 속성이 생기면 서브클래스에서 추가

## 클래스 정의

```cpp
class Sample {
public:
    // Constructors
    Sample();
    Sample(const std::string& sampleId,
           const std::string& name,
           double avgProductionTime,
           double yieldRate,
           int stock);

    // Getters
    std::string getSampleId() const;
    std::string getName() const;
    double getAvgProductionTime() const;
    double getYieldRate() const;
    int getStock() const;

    // Setters
    void setStock(int stock);

    // Business Logic
    bool hasEnoughStock(int requiredQty) const;
    int getShortfall(int requiredQty) const;       // 부족분 = requiredQty - stock (음수면 0)
    StockStatus getStockStatus(int pendingQty) const; // 여유/부족/고갈 판단

    // Utility
    std::string toString() const;

private:
    std::string sampleId_;
    std::string name_;
    double avgProductionTime_;  // min/ea
    double yieldRate_;          // 0.0 ~ 1.0
    int stock_;
};
```

## StockStatus (enum)

```cpp
enum class StockStatus {
    SUFFICIENT,  // 여유: 주문 대비 재고 충분
    SHORTAGE,    // 부족: 주문 대비 재고 부족
    DEPLETED     // 고갈: 재고 0
};
```

## 멤버 상세

| 멤버 | 타입 | 설명 | 제약 |
|---|---|---|---|
| sampleId_ | string | 시료 고유 ID | 비어있으면 안 됨, 중복 불가 |
| name_ | string | 시료 이름 | 비어있으면 안 됨 |
| avgProductionTime_ | double | 평균 생산시간 (min/ea) | 0 초과 |
| yieldRate_ | double | 수율 | 0.0 초과 ~ 1.0 이하 |
| stock_ | int | 현재 재고 수량 | 0 이상 |

## 주요 메서드 동작

### hasEnoughStock
```
재고(stock_) >= requiredQty → true
재고(stock_) < requiredQty  → false
```

### getShortfall
```
부족분 = requiredQty - stock_
결과가 음수면 0 반환 (재고가 충분한 경우)
```

### getStockStatus
```
stock_ == 0              → DEPLETED
stock_ < pendingQty      → SHORTAGE
stock_ >= pendingQty     → SUFFICIENT
```
