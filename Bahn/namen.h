// all names max 20 else %$@
const char nixx[] PROGMEM = "";

const char s_0[] PROGMEM = "Stepper";
const char s_1[] PROGMEM = "Horiz";
const char s_2[] PROGMEM = "Vert";
const char s_3[] PROGMEM = "";
const char s_4[] PROGMEM = "AusGreif";
const char s_5[] PROGMEM = "AusDreh";
const char s_6[] PROGMEM = "FeedRaus";
const char s_7[] PROGMEM = "FeedRein";
const char s_8[] PROGMEM = "";
const char s_9[] PROGMEM = "";
const char *const servNam[anzServ] PROGMEM = {s_0, s_1, s_2, s_3, s_4, s_5, s_6, s_7};

//positions
const char p_00[] PROGMEM = "Null";
const char p_01[] PROGMEM = "Feeder";
const char p_02[] PROGMEM = "Stat1";
const char p_03[] PROGMEM = "Stat2";
const char p_04[] PROGMEM = "Stat3";
const char p_05[] PROGMEM = "Stat4";
const char p_06[] PROGMEM = "stat5";
const char p_07[] PROGMEM = "?";
const char p_08[] PROGMEM = "Raus";
const char p_09[] PROGMEM = "Max";
//Horiz
const char p_10[] PROGMEM = "Min";
const char p_11[] PROGMEM = "Eingef";
const char p_12[] PROGMEM = "MovInnen";
const char p_13[] PROGMEM = "Bereit";
const char p_14[] PROGMEM = "LadeFeed";
const char p_15[] PROGMEM = "Entlad";
const char p_16[] PROGMEM = "Loop";
const char p_17[] PROGMEM = "string 7<";
const char p_18[] PROGMEM = "Raus";
const char p_19[] PROGMEM = "Max";
//Vert
const char p_20[] PROGMEM = "Min";
const char p_21[] PROGMEM = "Eingef";
const char p_22[] PROGMEM = "Stat1";
const char p_23[] PROGMEM = "Bereit";
const char p_24[] PROGMEM = "LadeFeed";
const char p_25[] PROGMEM = "Entlad";
const char p_26[] PROGMEM = "Loop";
const char p_27[] PROGMEM = "untRaus";
const char p_28[] PROGMEM = "Raus";
const char p_29[] PROGMEM = "Max";
//AusGreif
const char p_40[] PROGMEM = "Min";
const char p_41[] PROGMEM = "fest";
const char p_42[] PROGMEM = "leicht";
const char p_43[] PROGMEM = "deutlich";
const char p_44[] PROGMEM = "ganz";
const char p_45[] PROGMEM = "Entlad";
const char p_46[] PROGMEM = "Loop";
const char p_47[] PROGMEM = "untRaus";
const char p_48[] PROGMEM = "Raus";
const char p_49[] PROGMEM = "Max";
//FeedRaus
const char p_60[] PROGMEM = "Min";
const char p_61[] PROGMEM = "Hinten";
const char p_62[] PROGMEM = "";
const char p_63[] PROGMEM = "";
const char p_64[] PROGMEM = "Vorn";
const char p_65[] PROGMEM = "";
const char p_66[] PROGMEM = "";
const char p_67[] PROGMEM = "";
const char p_68[] PROGMEM = "";
const char p_69[] PROGMEM = "Max";
//FeedRein
const char p_70[] PROGMEM = "Min";
const char p_71[] PROGMEM = "Hinten";
const char p_72[] PROGMEM = "";
const char p_73[] PROGMEM = "";
const char p_74[] PROGMEM = "Vorn";
const char p_75[] PROGMEM = "";
const char p_76[] PROGMEM = "";
const char p_77[] PROGMEM = "";
const char p_78[] PROGMEM = "";
const char p_79[] PROGMEM = "Max";

const char *const posNam[anzServ][10] PROGMEM = {p_00, p_01, p_02, p_03, p_04, p_05, p_06, p_07, p_08, p_09,
                                                 p_10, p_11, p_12, p_13, p_14, p_15, p_16, p_17, p_18, p_19,
                                                 p_20, p_21, p_22, p_23, p_24, p_25, p_26, p_27, p_28, p_29,
                                                 nixx, nixx, nixx, nixx, nixx, nixx, nixx, nixx, nixx, nixx,
                                                 p_40, p_41, p_42, p_43, p_44, p_45, p_46, p_47, p_48, p_49,
                                                 nixx, nixx, nixx, nixx, nixx, nixx, nixx, nixx, nixx, nixx,
                                                 p_60, p_61, p_62, p_63, p_64, p_65, p_66, p_67, p_68, p_69,
                                                 p_70, p_71, p_72, p_73, p_74, p_75, p_76, p_77, p_78, p_79,
                                                };

// progs
const char pr_00[] PROGMEM = " ";
const char pr_01[] PROGMEM = "Grundst";
const char pr_02[] PROGMEM = "to Feed";
const char pr_03[] PROGMEM = "Feed to Move";
const char pr_04[] PROGMEM = "Entladen";
const char pr_05[] PROGMEM = "to Loop";
const char pr_06[] PROGMEM = "Laden";
const char pr_07[] PROGMEM = "Dreh Grundst";
const char pr_08[] PROGMEM = "Raus";
const char pr_09[] PROGMEM = "Zu & Rum";
const char pr_10[] PROGMEM = "to Manual";
const char pr_11[] PROGMEM = "Feed Rein";
const char pr_12[] PROGMEM = "Feed Raus";
const char *const progNam[20] PROGMEM = {pr_00, pr_01, pr_02, pr_03, pr_04, pr_05, pr_06, pr_07, pr_08, pr_09, pr_10, pr_11, pr_12, nixx, nixx, nixx, nixx, nixx, nixx, nixx};
