![Logo](Goldleaf/RomFs/Logo.png)

![License](https://img.shields.io/badge/License-GPLv3-blue.svg) [![Releases](https://img.shields.io/github/downloads/xortroll/goldleaf/total.svg)]() [![LatestVer](https://img.shields.io/github/release-pre/xortroll/goldleaf.svg)]()

> Goldleaf는 닌텐도 스위치 용 다목적 홈브류 도구입니다.

# 목차

1. [기능](#features)
2. [면책조항](#disclaimer)
3. [포워딩](#forwarding)
4. [구성](#configuration)
5. [알려진 버그](#known-bugs)
6. [크레딧과 지원](#credits-and-support)

## **[여기에서 다운로드하십시오!](https://github.com/XorTroll/Goldleaf/releases)**

<p>
<a href="https://www.patreon.com/xortroll"><img src="https://c5.patreon.com/external/logo/become_a_patron_button@2x.png" height="50" width="220"></a>
<br>
<b>당신이 내 일을 좋아한다면, 후원자가 되어 지지하는 것을 고려하십시오!</b>
</p>

## 기능

Goldleaf는 [FBI](https://github.com/Steveice10/FBI) 와 매우 비슷한 개념을 가지고 있는데 이는 닌텐도 3DS 홈브류와 유사한 도구입니다.

다음은 주요 기능입니다:

- **SD 카드 브라우저**:  (NSP, NRO, NACP, NXTheme, JPEG, 티켓 등)의 여러 파일 형식을 지원하고 복사, 붙여 넣기, 이름 바꾸기, 삭제, 파일과 디렉토리 생성을 지원하는 SD 카드 용 간단하지만 완벽한 파일 브라우저

- **콘솔 메모리 브라우저**: 동일한 브라우저이지만 낸드 파티션과 함께 사용하면 위험할 수 있으므로 내용을 쓰거나 삭제할 때 경고 메시지를 표시합니다.

- **USB 설치 프로그램**: USB 연결을 통해 **Goldtree** 또는 다른 PC 클라이언트에서 NSP를 전송하여 콘솔에 설치할 수 있습니다. 이 기능은 불안정하며 버그 및 문제에 직면할 수 있습니다.

- **콘텐츠 관리자**: SD 카드, 콘솔 메모리 또는 게임 카트리지의 모든 종류의 콘텐츠를 탐색합니다. 모든 NCAs를 탐색할 수 있으며 타이틀 옵션으로 컨텐츠를 제거하거나 NSP로 내보낼 수 있습니다. NAND 시스템 컨텐츠 (시스템 타이틀 또는 컨텐츠)는 삭제할 수 없습니다. 업데이트 또는 DLC와 같은 비공식적인 내용은 개별적으로 제거할 수 있습니다

- **티켓 매니저**: 일반 티켓과 개인 티켓을 모두 찾아보십시오. 삭제할 수는 있지만 일부 콘텐츠에서 사용중인 티켓을 제거하려고 할 때 추가 경고가 표시됩니다.

- **웹 애플릿을 통한 인터넷**: 콘솔의 웹 애플릿을 사용하여 Goldleaf는 웹 페이지를 검색하고 탐색할 수 있습니다. Goldleaf가 hbmenu를 통해 NRO로 시작되면 WifiWebAuth 애플릿이 사용됩니다. 이 애플릿은 때로는 실패하는 경향이 있으며 비디오 지원 부족과 같은 기능이 적습니다.

- **사용자 계정 관리자**: 사용자 계정을 선택하고, 사용자의 별명을 쉽게 변경하거나 심지어 삭제할 수 있습니다. 파일 브라우저를 통해 사용자의 아이콘을 JPEG 이미지로 바꿀 수는 있지만 위험한 행동 일 수 있습니다. 아이콘은 256x256 크기의 유효한 JPEG이어야합니다. Goldleaf는 아이콘이 다른 아이콘을 대체할 수 있는지 확인합니다.

- **콘솔 정보**: 현재 펌웨어 문자열 및 SD 카드 및 콘솔 메모리의 총 여유 공간 및 점유 공간을 볼 수있는 간단한 메뉴.

이 모든 기능 중에서 Goldleaf는 멋진 UI와 매우 직관적인 도구로 5 개 언어로 번역되었습니다: **영어**, **스페인어**, **독일어**, **프랑스어**, **이탈리아어**.

### 일반적인 컨트롤

- **(L-스틱 / D-패드)** 메뉴를 통해 이동

- **(R-스틱)** 메뉴를 통해 (매우) 빠르게 이동

- **(ZL / ZR)** 콘솔을 종료하거나 다시 부팅하는 대화 상자를 표시

- **(+ / -)** [NRO 전용] hbmenu로 돌아가려면 응용 프로그램을 종료

## 면책조항

### NSP 설치

항상 NSP를 설치하는 것이 금지되는 매우 쉬운 방법 일 수 있다는 것을 명심하십시오. 비행기 모드와 90DNS와 같은 특수 DNS를 사용하는 경우 금지될 가능성이 적습니다.

절대로 신뢰할 수 없는 NSP를 설치하지 마십시오. Goldleaf는 NSP가 홈브류 NSP (일반적으로 포워더 용으로 "05"로 시작하는 ID) 또는 정규 제목 (공식 ID 범위는 "01"로 시작하는 ID 만 허용)을 (NSP의 신청서 ID에 따른) 식별할 수 있습니다.

### 티켓

티켓은 게임 구매를 나타내지만, 기술적으로 말하면 티켓이 없으면 타이틀을 부팅할 수 없습니다 (타이틀에 티켓이 필요한 경우).

티켓이 없거나 요구하지 않는 NSP는 "표준 암호화" NSP입니다. 표준 암호화가 공식이 아닙니다 (일반적으로 티켓을 가져오는 것을 피하기 위해 재변환 된 내용).

Goldleaf는 티켓이 있는지 여부를 표시하고, 티켓이 있는 경우 티켓을 표시합니다. 타이틀 키는 티켓이 필요한 타이틀의 컨텐츠를 해독하고 접속해야 합니다. 그렇지 않으면 시스템에서 해독할 수 없으므로 시스템에서 실행을 중지할 수 있습니다.

## 포워딩

> **중요!** *이것은 0.6 이후의 릴리즈의 일부가 될 것입니다. 이 정보는 0.5 또는 0.5 버전의 Goldleaf에는 적용되지 않습니다.*

Goldleaf 자체는 일반적인 홈브류 프로젝트이므로 간단한 NRO입니다. 그러나 포워딩 프로젝트 시스템을 통해 Goldleaf를 설치된 타이틀 (포워더 NSP 설치)로 시작하거나 홈 메뉴를 대체 할 수 있습니다!

### 포워더 프로젝트

포워딩 프로세스는 일반적이며 간단합니다: Goldleaf의 NRO를 찾아 argc/argv를 통해 실행하십시오. 사용할 수 있는 포워더는 다음과 같습니다:

- **일반 포워더**: 정상적으로 Goldleaf NRO를 찾아 실행합니다. 설치 가능한 NSP로 구성되며 ApplicationId (일명 TitleID)가 `050032A5CF12E000` 입니다.

- **hbmenu(ish) 포워더**: Goldleaf NRO를 찾아서 실행하지만, hbmenu 모드에서는 홈브류 NRO 브라우징 및 실행을 위해 Goldleaf가 다른 초기 메뉴, UI...를 표시합니다. 이것은 ApplicationId (별명 TitleID)가 `0500E83A507FE000` 인 설치 가능한 NSP와, * hbmenu.nro *로 대체할 수 있는 단일 NRO에 공통으로 사용되는 대신 hbmenu로 Goldleaf를 사용합니다.

- **홈 메뉴 포워더 (LFS 대체)**: 이것은 사용되는 CFW에 적용되는 간단한 ExeFs 대체이며 Goldleaf의 NRO를 찾아 시작합니다. Goldleaf는 동일한 UI를 표시하지만 Hbmenu 포워더와 같은 특별한 UI가 곧 만들어집니다.

## 구성

Goldleaf는 Goldleaf의 폴더에 있는 INI 파일을 통해 색상, 자산 및 기타 옵션의 사용자 정의를 지원합니다: `sd:/goldleaf/goldleaf.ini`.

### 옵션

다음은 INI 파일에서 지원되는 옵션입니다 (찾을 수없는 부울 옵션의 기본값은 false입니다):

| 섹션    | 옵션                 | 설명                                                                                 |
|---------|----------------------|--------------------------------------------------------------------------------------|
| 일반    | useCustomLanguage    | { true, false }  true가 아니면 'customLanguage'가 무시됩니다.                        |
| 일반    | customLanguage       | { en, es, de, fr, it } 콘솔의 언어를 무시하고이 언어를 사용합니다.                   |
| 일반    | keysPath             | { ("/"로 시작하는 경로) } 키 파일 경로, 기본값은 "/switch/prod.keys" 입니다.         |
| UI      | romfsReplace         | { true, false } true가 아니라면 'romfsReplacePath'는 무시됩니다.                     |
| UI      | romfsReplacePath     | { ("/"로 시작하는 경로) } RomFs 리소스가 유효한 경우 검색 경로입니다.                |
| UI      | useCustomColors      | { true, false } ture가 아니라면 색상의 옵션은 무시됩니다.                            |
| UI      | colorBackground      | { (4 바이트 색상, 예: "55,125,255,255") } 배경색.                                    |
| UI      | colorBase            | { (4 바이트 색상, 예: "55,125,255,255") } 기본 표시 색상.                            |
| UI      | colorBaseFocus       | { (4 바이트 색상, 예: "55,125,255,255") } 초점을 맞출 때의 기본 표시 색상.           |
| UI      | colorText            | { (4 바이트 색상, 예: "55,125,255,255") } 문자색.                                    |
| UI      | useCustomSizes       | { true, false } true가 아니면 크기 옵션이 무시됩니다.                                |
| UI      | fileBrowserItemsSize | { (5로 나눌 수있는 숫자) } 파일 브라우저의 항목 크기, 기본적으로 50입니다.           |

### 참고

예를 들어 Goldflaf가`romfs:/FileSystem/FileDataFont.ttf` 리소스 폰트를 찾을 때 RomFs 대체를 통해, `romfsReplace`가 true이고`romfsReplacePath`가 `/goldleaf/testromfs` 인 경우 Goldleaf는 `sd:/goldleaf/testromfs/FileSystem/FileDataFont.ttf`가 있는지 확인하고 사용한다면 RomFs를 사용하십시오.

따라서 이 구성을 통해 UI의 이미지, 리소스, 요소 크기 및 심지어 번역 (사용자 정의 JSON 번역 사용)을 사용할 수 있을 뿐 아니라 향후 업데이트에서 추가되는 자산이 더 많이 사용될 수 있습니다.

## 알려진 버그

- Atmosphère 및 SX OS에서 홈 메뉴를 통해 Goldleaf를 종료하면 (NRO로) 7.x 펌웨어에서 시스템이 중단됩니다.

- USB 설치에는 일반적으로 UI 시스템과 관련된 몇 가지 버그가있는 것으로 보입니다.

## 크레딧과 지원

### 크레딧

Goldleaf의 주요 개념은 XorTroll에 의해 개발되었지만 다른 많은 노력과 지원이 없으면 이 프로젝트는 지금과 같지 않을 것입니다:

(아래 열거된 모든 사람들로부터 그들 각자의 홈브류 지역에 거대한 지지를 얻는 Adubbz, exelix, C4Phoenix, The-4n, SciresM에게 특별한 감사를 전합니다.)

- Adubbz와 모든 (오래된) [Tinfoil](https://github.com/Adubbz/Tinfoil) 기여자, 타이틀 설치와 함께 그들의 거대한 작업.

- exelix과 Qcean 팀, 협력하여 홈 메뉴 테마를 지원. Goldleaf는 (설치한) [SwitchThemesCommon](https://github.com/exelix11/SwitchThemeInjector) 라이브러리를 사용하여 테마 설치 처리.

- C4Phoenix, 이 프로젝트의 로고를 작성하는 그의 멋진 작업과 설치된 버전을 실행할 때 GIF가 표시

- Goldleaf의 것을 제외한 모든 아이콘 (위의 크레딧 참조)은 [Icons8](https://icons8.com) 에서 가져 왔습니다.

- 2767mr, USB를 보다 안정적으로 설치하기 위한 모든 지원과 작업중인 Goldtree의 .NET 코어 버전을 제공.

- The-4n, [hacBrewPack](https://github.com/The-4n/hacBrewPack) 에서 완전히 합법적 인 NSP를 만들 수 있습니다.

- SciresM, [hactool](https://github.com/SciresM/hactool) 은 NCA 추출을 Goldleaf에서하는 라이브러리로 포팅.

- Thealexbarney, 다양한 닌텐도 스위치 형식의 C# 라이브러리: LibHac, Goldtree에서 사용.

- Simon, Goldtree 클라이언트를 가능하게 만든 C#의 libusbK 구현.

- shchmue and blawar for the system to get titlekeys without breaking processes, found in [Lockpick](https://github.com/shchmue/Lockpick).

- 번역가: [unbranched](https://github.com/unbranched) 와 [exelix](https://github.com/exelix11) 는 이탈리아어, [tiliarou](https://github.com/tiliarou) 와 [FuryBaguette](https://github.com/FuryBaguette) 는 프랑스어, [LoOkYe](https://github.com/lookye) 는 독일어. (영어와 스페인어는 모두 내가 제작.)

- 버그를 보고하고 프로젝트 개발에 많은 도움을 준 모든 테스터.

### Discord 서버

내 프로젝트의 상태와 지원에 대해 더 알고 싶으면 Discord 서버 인 [H&H](https://discord.gg/3KpFyaH) 를 확인해야합니다. 내 프로젝트에 중점을 둔 홈브류 및 해킹을 위한 간단한 서버입니다. 베타 테스터가 되고 싶다면 테스터를 위한 나이트 빌드 시스템에 관심이 있을 것입니다.
