# Class Design: SampleController

## 개요

시료(Sample) 관련 비즈니스 로직 처리 담당.  
View로부터 입력을 받아 Repository를 통해 데이터를 처리한다.

## 클래스 정의

```cpp
class SampleController {
public:
    explicit SampleController(SampleRepository& sampleRepo);

    // 시료 등록
    bool registerSample(const std::string& sampleId,
                        const std::string& name,
                        double avgProductionTime,
                        double yieldRate,
                        int initialStock);

    // 시료 조회
    std::vector<Sample> getAllSamples();
    std::optional<Sample> getSampleById(const std::string& sampleId);
    std::vector<Sample> searchSamplesByName(const std::string& keyword);

    // 재고 관련
    bool updateStock(const std::string& sampleId, int newStock);
    bool increaseStock(const std::string& sampleId, int amount);
    bool decreaseStock(const std::string& sampleId, int amount);

    // 유효성 검사
    bool existsSample(const std::string& sampleId);

private:
    SampleRepository& sampleRepo_;
    bool isValidYieldRate(double yieldRate) const;        // 0.0 < yieldRate <= 1.0
    bool isValidProductionTime(double time) const;        // time > 0
};
```

## 주요 메서드 동작

### registerSample
```
1. sampleId 중복 체크 → 중복이면 false 반환
2. 입력값 유효성 검사 (수율 범위, 생산시간 양수 등)
3. Sample 객체 생성 후 Repository에 저장
```

### decreaseStock
```
1. 현재 재고 조회
2. 재고 >= amount 확인
3. newStock = 현재재고 - amount 후 업데이트
```
