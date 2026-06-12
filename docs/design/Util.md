# Class Design: Util Layer

## 포함 클래스
- DBManager
- IdGenerator
- DateTimeUtil

---

## 1. DBManager

### 개요
SQLite DB 연결을 관리하는 싱글톤 유틸리티.
모든 Repository는 DBManager를 통해 DB 커넥션을 획득한다.

### 클래스 정의
```cpp
class DBManager {
public:
    static DBManager& getInstance();

    bool initialize(const std::string& dbPath = "data/sample_order.db");
    void close();
    sqlite3* getConnection();
    bool isConnected() const;
    bool createTables();

    DBManager(const DBManager&) = delete;
    DBManager& operator=(const DBManager&) = delete;

private:
    DBManager() = default;
    ~DBManager();

    sqlite3* db_ = nullptr;
    bool connected_ = false;
};
```

### 초기화 흐름
```
main()
  └─ DBManager::getInstance().initialize("data/sample_order.db")
       ├─ sqlite3_open(dbPath)
       └─ createTables()
            ├─ CREATE TABLE IF NOT EXISTS samples (...)
            ├─ CREATE TABLE IF NOT EXISTS orders (...)
            └─ CREATE TABLE IF NOT EXISTS production_queue (...)
```

---

## 2. IdGenerator

### 개요
주문번호 등 ID 생성을 담당. 인스턴스화 불가 (static only).

### 클래스 정의
```cpp
class IdGenerator {
public:
    static std::string generateOrderId(int sequence);  // ORD-YYYYMMDD-NNNN
    static std::string generateSampleId(int sequence); // S-NNN

private:
    IdGenerator() = delete;
};
```

### 동작 예시
```
generateOrderId(1)   → "ORD-20260612-0001"
generateSampleId(3)  → "S-003"
```

---

## 3. DateTimeUtil

### 개요
날짜/시간 처리 유틸리티. 인스턴스화 불가 (static only).

### 클래스 정의
```cpp
class DateTimeUtil {
public:
    static std::string now();    // 현재 시각 ISO 8601
    static std::string today();  // 오늘 날짜 YYYYMMDD

private:
    DateTimeUtil() = delete;
};
```

### 동작 예시
```
DateTimeUtil::now()   → "2026-06-12T09:32:15"
DateTimeUtil::today() → "20260612"
```
