# Class Design: SampleView

## 개요

시료 관리 화면 출력 및 입력 처리 담당.

## 클래스 정의

```cpp
class SampleView {
public:
    explicit SampleView(SampleController& sampleCtrl);

    void show();                        // 시료 관리 서브메뉴 루프
    void displayMenu();
    void showRegisterForm();            // 시료 등록 폼
    void showSampleList();              // 시료 목록 출력
    void showSearchForm();              // 시료 검색 폼 + 결과 출력
    void displaySample(const Sample& sample);
    void displaySampleTable(const std::vector<Sample>& samples);

private:
    SampleController& sampleCtrl_;
};
```

## 화면 구성 - 시료 목록

```
---------------------------------------------------------------
  [1] 시료 관리
  [1] 시료 등록   [2] 시료 목록   [3] 시료 검색   [0] 뒤로
  선택 > 2

  등록 시료 목록  (총 12종)
  ID       시료명                평균 생산시간   수율    현재 재고
  S-001    실리콘 웨이퍼-8인치   0.5 min/ea     0.92    480 ea
  S-002    GaN 에피택셀-4인치    0.3 min/ea     0.78    220 ea
  ...
```
