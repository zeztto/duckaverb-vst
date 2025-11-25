# DUCKAVERB Development Guide

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

DUCKAVERB의 핵심 오디오 처리는 다음 단계로 이루어집니다:

1. **입력 레벨 감지** (Sample 162-170):
   - Peak 레벨 감지: 각 샘플의 절대값 측정
   - 스테레오의 경우 좌우 채널 중 큰 값 사용
   - 기타 연주의 다이나믹 범위 실시간 추적

2. **덕킹 엔벨로프 생성** (Sample 173-177):
   - **Attack/Release 엔벨로프 팔로워**:
     - Attack: 1ms (즉각적 반응) - 연주 시작 시 리버브 빠르게 억제
     - Release: 50-200ms (Space 값에 비례) - 연주 멈춤 후 리버브 서서히 회복
   - 부드러운 곡선으로 갑작스러운 변화 방지

3. **덕킹 게인 계산** (Sample 179-195):
   ```cpp
   float duckingGain = 1.0f;  // 기본: 100% 리버브
   if (currentEnv > threshold) {
     // Threshold 초과량 계산 (0.0 ~ 1.0)
     float excessAmount = (currentEnv - threshold) / threshold;
     excessAmount = juce::jmin(excessAmount, 1.0f);  // [중요] 1.0으로 제한

     // Soft-knee 보간: 1.0 (full) → 1/ratio (reduced)
     float minGain = 1.0f / ratio;  // ratio 3:1 → 0.33, 8:1 → 0.125
     duckingGain = 1.0f - (excessAmount * (1.0f - minGain));

     // 최소 1% 유지 (완전히 무음 방지)
     duckingGain = juce::jmax(0.01f, duckingGain);
   }
   ```
   - **연주 중**: 리버브 18-33%로 감소 (Space 값에 따라)
   - **연주 멈춤**: 리버브 100%로 회복
   - **Soft-knee**: Threshold 근처에서 부드럽게 전환

4. **리버브 처리** (Sample 131-147):
   - JUCE의 `juce::Reverb` 사용
   - 100% wet으로 처리 후 수동 믹싱
   - Room Size, Damping, Width 파라미터를 Space 노브로 통합 제어

5. **믹싱** (Sample 197-200):
   - 드라이 신호 + 덕킹된 웨트 신호
   - Space 노브로 Wet Level (0-70%) 제어
   - Volume Compensation (1.0x-1.5x) 적용

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
juce_add_plugin(DUCKAVERB
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
   open build/DUCKAVERB.xcodeproj
   ```

2. **Standalone 앱 실행**:
   ```bash
   build/DUCKAVERB_artefacts/Release/Standalone/DUCKAVERB.app/Contents/MacOS/DUCKAVERB
   ```

3. **로그 확인**:
   ```bash
   log stream --predicate 'process == "DUCKAVERB"'
   ```

### Windows

1. Visual Studio에서 `build/DUCKAVERB.sln` 열기
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
   project(DUCKAVERB VERSION 0.0.2)
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
  build/DUCKAVERB_artefacts/Release/AU/DUCKAVERB.component
```

## 참고 자료

- [JUCE Documentation](https://docs.juce.com/)
- [JUCE Forum](https://forum.juce.com/)
- [CMake Documentation](https://cmake.org/documentation/)
- [fuzza-vst](https://github.com/zeztto/fuzza-vst) - 참고 프로젝트
