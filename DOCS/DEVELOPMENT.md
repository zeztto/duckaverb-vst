# Duckaverb Development Guide

## 프로젝트 구조

```
duckaverb-vst/
├── Source/
│   ├── PluginProcessor.cpp    # DSP 처리 및 파라미터 관리
│   ├── PluginProcessor.h
│   ├── PluginEditor.cpp        # UI 렌더링 및 레이아웃
│   └── PluginEditor.h
├── DOCS/                       # 개발 문서
├── build/                      # 빌드 출력 (gitignored)
├── icon.png                    # 앱 아이콘
├── CMakeLists.txt              # CMake 빌드 설정
└── README.md
```

## 개발 환경 설정

### macOS

1. **Xcode 설치**:
   ```bash
   xcode-select --install
   ```

2. **CMake 설치**:
   ```bash
   brew install cmake
   ```

3. **프로젝트 클론 및 빌드**:
   ```bash
   git clone https://github.com/zeztto/duckaverb-vst.git
   cd duckaverb-vst
   cmake -B build
   cmake --build build --config Release
   ```

### Windows

1. **Visual Studio 2019 이상** 설치 (C++ 데스크톱 개발 워크로드 포함)
2. **CMake** 설치 (https://cmake.org/download/)
3. **프로젝트 빌드**:
   ```cmd
   git clone https://github.com/zeztto/duckaverb-vst.git
   cd duckaverb-vst
   cmake -B build -G "Visual Studio 16 2019"
   cmake --build build --config Release
   ```

## 아키텍처

### DSP 처리 (PluginProcessor.cpp)

Duckaverb의 핵심 오디오 처리는 다음 단계로 이루어집니다:

1. **입력 레벨 감지**:
   - RMS 레벨 계산으로 입력 신호의 강도 측정
   - 기타 연주의 다이나믹 범위 추적

2. **덕킹 엔벨로프 생성**:
   - Attack/Release 파라미터로 부드러운 엔벨로프 생성
   - 연주 중에는 리버브 레벨 감소, 연주 멈추면 증가

3. **리버브 처리**:
   - JUCE의 `juce::Reverb` 사용
   - Room Size, Damping, Width 파라미터를 Space 노브로 통합 제어

4. **믹싱**:
   - 드라이 신호와 덕킹된 웨트 신호 믹싱
   - Space 노브로 Dry/Wet 비율 제어

### UI (PluginEditor.cpp)

페달 스타일 UI 구성:

- **페인트 레이어**:
  1. 배경 그라데이션 및 테두리
  2. 로고 및 서브타이틀
  3. 노브 모듈 박스 (인셋 스타일)
  4. 코너 나사 디테일
  5. 푸터

- **컴포넌트**:
  - `spaceSlider`: 로터리 노브 (0.0 - 1.0)
  - `spaceLabel`: "SPACE" 라벨
  - `bypassButton`: 토글 버튼

## 파라미터

### Space (0.0 - 1.0)

단일 노브가 여러 파라미터를 동시에 제어:

| Space 값 | Room Size | Damping | Wet Level | Ducking Sensitivity |
|----------|-----------|---------|-----------|---------------------|
| 0.0      | 0.3       | 0.5     | 0.0       | 1.0                 |
| 0.5      | 0.6       | 0.35    | 0.35      | 0.5                 |
| 1.0      | 0.9       | 0.2     | 0.7       | 0.1                 |

### Bypass (Boolean)

- **OFF**: 이펙트 활성화
- **ON**: 드라이 신호만 출력

## 빌드 설정

### CMakeLists.txt 주요 설정

```cmake
juce_add_plugin(Duckaverb
    COMPANY_NAME "h4ppy Labs"
    PLUGIN_MANUFACTURER_CODE H4py
    PLUGIN_CODE Duck
    FORMATS AU VST3 Standalone
    ICON_BIG "${CMAKE_CURRENT_SOURCE_DIR}/icon.png"
    ICON_SMALL "${CMAKE_CURRENT_SOURCE_DIR}/icon.png"
)
```

### Clean 빌드

```bash
cmake --build build --config Release --clean-first
```

## 디버깅

### macOS

1. **Xcode에서 열기**:
   ```bash
   open build/Duckaverb.xcodeproj
   ```

2. **Standalone 앱 실행**:
   ```bash
   build/Duckaverb_artefacts/Release/Standalone/Duckaverb.app/Contents/MacOS/Duckaverb
   ```

3. **로그 확인**:
   ```bash
   log stream --predicate 'process == "Duckaverb"'
   ```

### Windows

1. Visual Studio에서 `build/Duckaverb.sln` 열기
2. Standalone 타겟 선택 후 F5로 디버깅

## 코드 스타일

- **들여쓰기**: 스페이스 2칸
- **네이밍**:
  - 클래스: PascalCase
  - 함수/변수: camelCase
  - 상수: UPPER_SNAKE_CASE
- **포맷**: JUCE 코딩 스타일 따름

## 테스트

### 수동 테스트 체크리스트

- [ ] Space 노브: 0%, 50%, 100%에서 사운드 확인
- [ ] Bypass 버튼 토글 테스트
- [ ] 입력 신호 레벨 변화에 따른 덕킹 동작 확인
- [ ] DAW에서 플러그인 로드 테스트 (Logic, Ableton, Reaper 등)
- [ ] Standalone 앱 정상 실행 확인
- [ ] 아이콘 표시 확인

## 배포

### 버전 업데이트

1. `CMakeLists.txt`에서 버전 번호 업데이트:
   ```cmake
   project(Duckaverb VERSION 0.0.2)
   ```

2. `CHANGELOG.md` 업데이트

3. Git 태그 생성:
   ```bash
   git tag -a v0.0.2 -m "Release v0.0.2"
   git push origin v0.0.2
   ```

### 릴리스 빌드

```bash
cmake --build build --config Release
```

### macOS 코드 사인 (옵션)

```bash
codesign --force --sign "Developer ID Application: YourName" \
  build/Duckaverb_artefacts/Release/AU/Duckaverb.component
```

## 참고 자료

- [JUCE Documentation](https://docs.juce.com/)
- [JUCE Forum](https://forum.juce.com/)
- [CMake Documentation](https://cmake.org/documentation/)
- [fuzza-vst](https://github.com/zeztto/fuzza-vst) - 참고 프로젝트
