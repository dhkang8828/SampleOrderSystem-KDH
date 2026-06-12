# Class Design: DBManager

## 개요

SQLite DB 연결을 관리하는 싱글톤 유틸리티.  
모든 Repository는 DBManager를 통해 DB 커넥션을 획득한다.

## 클래스 정의

```cpp
class DBManager {
public:
    static DBManager& getInstance();

    bool initialize(const std::string& dbPath = "data/sample_order.db");
    void close();
    sqlite3* getConnection();
    bool isConnected() const;

    // 테이블 초기화 (최초 실행 시)
    bool createTables();

    // 복사/이동 금지 (싱글톤)
    DBManager(const DBManager&) = delete;
    DBManager& operator=(const DBManager&) = delete;

private:
    DBManager() = default;
    ~DBManager();

    sqlite3* db_ = nullptr;
    bool connected_ = false;
};
```

## 초기화 흐름

```
main()
  └─ DBManager::getInstance().initialize("data/sample_order.db")
       ├─ sqlite3_open(dbPath)
       └─ createTables()
            ├─ CREATE TABLE IF NOT EXISTS samples (...)
            ├─ CREATE TABLE IF NOT EXISTS orders (...)
            └─ CREATE TABLE IF NOT EXISTS production_queue (...)
```
