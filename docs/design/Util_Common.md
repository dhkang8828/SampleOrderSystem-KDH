# Class Design: Utility Classes

## IdGenerator

주문번호 등 ID 생성을 담당하는 유틸리티.

```cpp
class IdGenerator {
public:
    // ORD-YYYYMMDD-NNNN 형식 주문번호 생성
    static std::string generateOrderId(int sequence);

    // S-NNN 형식 시료 ID 생성 (자동 생성 시)
    static std::string generateSampleId(int sequence);

private:
    IdGenerator() = delete;  // 인스턴스화 불가
};
```

### generateOrderId 동작
```
sequence = 1, 날짜 = 2026-06-12
→ "ORD-20260612-0001"
```

---

## DateTimeUtil

날짜/시간 처리 유틸리티.

```cpp
class DateTimeUtil {
public:
    static std::string now();                         // 현재 시각 ISO 8601
    static std::string today();                       // 오늘 날짜 YYYYMMDD
    static std::string format(const std::string& iso8601,
                               const std::string& fmt); // 형식 변환

private:
    DateTimeUtil() = delete;
};
```

### 사용 예시
```cpp
DateTimeUtil::now()    // "2026-06-12T09:32:15"
DateTimeUtil::today()  // "20260612"
```
