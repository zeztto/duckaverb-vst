# Changelog

All notable changes to the DUCKAVERB VST project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Planned
- 프리셋 시스템
- 사용자 정의 색상 테마
- 리사이즈 가능한 윈도우

## [0.0.2] - 2025-01-24

### Fixed
- **[Critical] 덕킹 리버브 알고리즘 버그 수정** (PluginProcessor.cpp:179-195)
  - **문제**: `excessdB` 변수가 무제한으로 증가하여 `duckingGain`이 음수가 되고 0으로 clamp되는 현상
  - **증상**: 연주 중 리버브가 의도한 것보다 과도하게 억제되거나 완전히 사라짐
  - **원인**: 입력 레벨이 threshold를 약간만 초과해도 게인 계산이 음수로 전환
  - **해결**:
    - `excessAmount`를 1.0으로 제한하여 안정적인 게인 감소 범위 확보
    - 최소 게인을 1%로 설정하여 자연스러운 사운드 유지
    - Soft-knee 방식의 부드러운 덕킹 전환 구현
  - **효과**:
    - 연주 중: 리버브가 적절히 억제되어 명료한 사운드 (18-33% 게인)
    - 연주 멈춤: 리버브가 release time에 따라 자연스럽게 차오름 (100% 게인)
    - 사용자 피드백에 따른 의도된 덕킹 리버브 효과 정상 구현

### Improved
- 덕킹 알고리즘 코드 주석 개선
  - 각 변수의 역할과 범위 명시
  - 계산 로직 설명 추가
- 게인 리덕션 안정성 향상
  - Ratio 파라미터가 예상대로 작동 (3:1 ~ 8:1)
  - Space 노브에 따른 민감도 조절 정상화

## [0.0.1] - 2024-11-24

### Added
- 초기 릴리스
- One-Knob "Space" 컨트롤로 리버브 파라미터 통합 제어
- Ducking Reverb 알고리즘 구현
  - RMS 레벨 기반 입력 감지
  - Attack/Release 엔벨로프로 부드러운 덕킹
  - Space 값에 따른 덕킹 감도 조절
- FUZZA VST 스타일의 페달 UI
  - 다크 테마 배경 (`#0f0f0f` ~ `#1f1f1f`)
  - 오렌지 악센트 서브타이틀 (`#ff9933`)
  - 파란색 SPACE 노브 (`#1e90ff`)
  - 인셋 패널 디자인
  - 코너 나사 장식
- BYPASS 풋스위치 버튼
  - OFF 상태: 회색 배경 (`#333333`)
  - ON 상태: 빨간색 배경 (`#dd3333`)
- 커스텀 앱 아이콘
  - 오리와 오디오 파형 디자인
  - 1024×1024 PNG → .icns 자동 변환
- macOS 지원
  - AU (Audio Units)
  - VST3
  - Standalone 앱
- JUCE 8.0.4 기반 개발
- CMake 빌드 시스템

### UI Improvements
- 깨진 한글 텍스트 제거 ("덕커버브" → "Ducking Reverb")
- 로고 크기 최적화 (52pt → 38pt)
- 컴팩트한 단일 노브 레이아웃
  - 노브 박스 좌우 60px 여백
  - SPACE 라벨 20pt
- BYPASS 버튼 크기 및 간격 조정
  - 크기: 150×45px
  - 노브 박스와 30px 간격
- 향상된 시각적 계층
  - 더 어두운 배경 그라데이션
  - 이중 테두리 (외부 검은색 + 내부 회색 하이라이트)
  - 3D 나사 디테일 (베이스, 하이라이트, 슬롯)
- FUZZA VST와 일관된 디자인 언어

### DSP Features
- Room Size: 0.3 ~ 0.9 (Space 값에 비례)
- Damping: 0.5 ~ 0.2 (Space 값에 반비례)
- Wet Level: 0.0 ~ 0.7 (Space 값에 비례, 최대 70%)
- Ducking Threshold: 0.02 ~ 0.10 (Space 값에 비례)
- Ducking Ratio: 3:1 ~ 8:1 (Space 값에 비례)
- Attack Time: 1ms (즉각적인 덕킹)
- Release Time: 50ms ~ 200ms (Space 값에 비례)
- Volume Compensation: 1.0x ~ 1.5x (Space 값에 비례, 약 +3.5dB)

### Documentation
- README.md 작성
  - 프로젝트 소개
  - 주요 기능 설명
  - 빌드 방법
  - 설치 경로 안내
- DEVELOPMENT.md 작성
  - 프로젝트 구조
  - 개발 환경 설정
  - 아키텍처 설명
  - 디버깅 가이드
  - 배포 프로세스
- UI_DESIGN.md 작성
  - 디자인 철학
  - 색상 팔레트
  - 레이아웃 가이드
  - 컴포넌트 상세
  - FUZZA VST와의 일관성
- CHANGELOG.md (이 파일)

### Technical
- CMakeLists.txt 설정
  - JUCE 8.0.4 FetchContent 사용
  - ICON_BIG 및 ICON_SMALL 설정
  - AU, VST3, Standalone 포맷 지원
  - 자동 플러그인 설치 (macOS)
- C++17 표준 사용
- JUCE 권장 컴파일 플래그 적용

### Build & Deployment
- GitHub 리포지토리 설정
- .gitignore 구성 (build/, IDE 파일 제외)
- Release 빌드 최적화

---

## 버전 히스토리

### Version Numbering

- **Major.Minor.Patch** (예: 1.0.0)
  - **Major**: 호환성 깨지는 변경사항
  - **Minor**: 새로운 기능 추가 (하위 호환)
  - **Patch**: 버그 수정 및 마이너 개선

### 업데이트 예정

다음 버전 (0.1.0)에서 계획된 기능:
- [ ] Windows 빌드 및 테스트
- [ ] 프리셋 시스템 (Small/Medium/Large Space)
- [ ] 파라미터 자동화 지원 개선
- [ ] CPU 사용량 최적화
- [ ] 추가 리버브 알고리즘 옵션

---

## 기여자

- **h4ppy Labs** - 초기 개발
- **Contributors** - (여기에 기여자 추가)

## 참고

- 이 프로젝트는 [fuzza-vst](https://github.com/zeztto/fuzza-vst)에서 영감을 받았습니다
- JUCE 프레임워크 사용: https://juce.com
