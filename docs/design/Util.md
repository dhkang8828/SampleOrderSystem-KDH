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
       ├─ std::filesystem::create_directories("data")  ← data/ 폴더 자동 생성
       ├─ sqlite3_open(dbPath)
       │    ├─ DB 파일 없음 → 새 DB 파일 생성
       │    └─ DB 파일 있음 → 기존 DB 그대로 열기 (이전 데이터 유지)
       └─ createTables()
            ├─ CREATE TABLE IF NOT EXISTS samples (...)
            ├─ CREATE TABLE IF NOT EXISTS orders (...)
            └─ CREATE TABLE IF NOT EXISTS production_queue (...)
```

### 데이터 영속성 보장
- `sqlite3_open()`은 파일이 존재하면 기존 DB를 열고, 없으면 새로 생성
- `CREATE TABLE IF NOT EXISTS`로 기존 데이터를 덮어쓰지 않음
- 프로그램 재실행 시 별도 로드 로직 없이 Repository의 `findAll()` 등이 자동으로 이전 데이터 조회

### initialize() 구현 시 주의사항
```cpp
bool DBManager::initialize(const std::string& dbPath) {
    // data/ 폴더가 없으면 자동 생성 (C++17 filesystem)
    std::filesystem::create_directories(
        std::filesystem::path(dbPath).parent_path()
    );
    if (sqlite3_open(dbPath.c_str(), &db_) != SQLITE_OK) {
        return false;
    }
    connected_ = true;
    return createTables();
}

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
