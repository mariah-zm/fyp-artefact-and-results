#include "rules.ih"

// Private Methods
void initCoffeeEngine(fl::Engine *engine);
void initWorkEngine(fl::Engine *engine);
void initPlayEngine(fl::Engine *engine);
void initEatEngine(fl::Engine *engine);
void initLectureEngine(fl::Engine *engine);
void initSpawningEngine(fl::Engine *engine);
void initDeactivateNpcEngine(fl::Engine *engine);

void initRulesEngine(RulesEngine *engine)
{
    engine->coffeeEngine = new fl::Engine;
    engine->workEngine = new fl::Engine;
    engine->playEngine = new fl::Engine;
    engine->eatEngine = new fl::Engine;
    engine->lectureEngine = new fl::Engine;
    engine->spawningEngine = new fl::Engine;
    engine->disableNpcEngine = new fl::Engine;

    initCoffeeEngine(engine->coffeeEngine);
    initWorkEngine(engine->workEngine);
    initPlayEngine(engine->playEngine);
    initEatEngine(engine->eatEngine);
    initLectureEngine(engine->lectureEngine);
    initSpawningEngine(engine->spawningEngine);
    initDeactivateNpcEngine(engine->disableNpcEngine);
}

void destroyRulesEngine(RulesEngine *engine)
{
    delete engine->coffeeEngine;
    delete engine->workEngine;
    delete engine->playEngine;
    delete engine->eatEngine;
    delete engine->lectureEngine;
    delete engine->spawningEngine;
    delete engine->disableNpcEngine;
}

void initCoffeeEngine(fl::Engine *engine)
{
    fl::InputVariable *energy = new fl::InputVariable;
    energy->setName(ENERGY_INPUT);
    energy->setRange(0.0, 1.0);
    energy->addTerm(new fl::Trapezoid("LOW", 0.0, 0.0, 0.2, 0.35));
    energy->addTerm(new fl::Trapezoid("MEDIUM", 0.3, 0.4, 0.7, 0.75));
    energy->addTerm(new fl::Trapezoid("HIGH", 0.7, 0.75, 1.0, 1.0));
    engine->addInputVariable(energy);

    fl::OutputVariable *coffee = new fl::OutputVariable;
    coffee->setName(COFFEE_OUTPUT);
    coffee->setRange(0.0, 1.0);
    coffee->setDefaultValue(fl::nan);
    coffee->setDefuzzifier(new fl::WeightedAverage("Automatic"));
    coffee->addTerm(new fl::Triangle("NO", 0, 0, 0.55));
    coffee->addTerm(new fl::Triangle("YES", 0.45, 1, 1));
    engine->addOutputVariable(coffee);

    fl::RuleBlock *rules = new fl::RuleBlock;
    rules->setName("Coffee Rules");
    rules->addRule(fl::Rule::parse("if Energy is LOW then Coffee is YES", engine));
    rules->addRule(fl::Rule::parse("if Energy is MEDIUM then Coffee is NO", engine));
    rules->addRule(fl::Rule::parse("if Energy is HIGH then Coffee is NO", engine));
    engine->addRuleBlock(rules);
}

void initWorkEngine(fl::Engine *engine)
{
    fl::InputVariable *energy = new fl::InputVariable;
    energy->setName(ENERGY_INPUT);
    energy->setRange(0.0, 1.0);
    energy->addTerm(new fl::Trapezoid("LOW", 0.0, 0.0, 0.2, 0.35));
    energy->addTerm(new fl::Trapezoid("MEDIUM", 0.3, 0.4, 0.7, 0.75));
    energy->addTerm(new fl::Trapezoid("HIGH", 0.7, 0.75, 1.0, 1.0));
    engine->addInputVariable(energy);

    fl::InputVariable *productivity = new fl::InputVariable;
    productivity->setName(PRODUCTIVITY_INPUT);
    productivity->setRange(0.0, 1.0);
    productivity->addTerm(new fl::Trapezoid("LOW", 0.0, 0.0, 0.2, 0.35));
    productivity->addTerm(new fl::Trapezoid("MEDIUM", 0.3, 0.4, 0.7, 0.75));
    productivity->addTerm(new fl::Trapezoid("HIGH", 0.7, 0.75, 1.0, 1.0));
    engine->addInputVariable(productivity);

    fl::InputVariable *drivenTrait = new fl::InputVariable;
    drivenTrait->setName(DRIVEN_INPUT);
    drivenTrait->setRange(-1, 1);
    drivenTrait->addTerm(new fl::Trapezoid("VERY_LOW", -1, -1, -0.7, -0.5));
    drivenTrait->addTerm(new fl::Trapezoid("LOW", -0.6, -0.4, 0, 0.1));
    drivenTrait->addTerm(new fl::Trapezoid("MODERATE", 0, 0.1, 0.3, 0.4));
    drivenTrait->addTerm(new fl::Trapezoid("HIGH", 0.3, 0.4, 0.5, 0.6));
    drivenTrait->addTerm(new fl::Trapezoid("VERY_HIGH", 0.5, 0.6, 1, 1));
    engine->addInputVariable(drivenTrait);

    fl::OutputVariable *studyWork = new fl::OutputVariable;
    studyWork->setName(STUDY_WORK_OUTPUT);
    studyWork->setRange(0.0, 1.0);
    studyWork->setDefaultValue(fl::nan);
    studyWork->setAggregation(new fl::AlgebraicSum);
    studyWork->setDefuzzifier(new fl::WeightedAverage("Automatic"));
    studyWork->addTerm(new fl::Triangle("NO", 0, 0, 0.55));
    studyWork->addTerm(new fl::Triangle("YES", 0.45, 1, 1));
    engine->addOutputVariable(studyWork);

    fl::RuleBlock *rules = new fl::RuleBlock;
    rules->setName("Work Rules");
    rules->setConjunction(new fl::AlgebraicProduct);
    rules->setDisjunction(new fl::AlgebraicSum);
    rules->addRule(fl::Rule::parse("if Productivity is HIGH and (Driven is VERY_LOW or Driven is LOW or Driven is MODERATE or Driven is HIGH) then Work is NO", engine));
    rules->addRule(fl::Rule::parse("if Productivity is HIGH and (Energy is MEDIUM or Energy is LOW) and Driven is VERY_HIGH then Work is NO", engine));
    rules->addRule(fl::Rule::parse("if Energy is LOW and Productivity is MEDIUM and (Driven is VERY_LOW or Driven is LOW or Driven is MODERATE) then Work is NO", engine));
    rules->addRule(fl::Rule::parse("if Energy is LOW and Productivity is LOW and (Driven is VERY_LOW or Driven is LOW) then Work is NO", engine));
    rules->addRule(fl::Rule::parse("if Energy is MEDIUM and Productivity is MEDIUM and Driven is VERY_LOW then Work is NO", engine));

    rules->addRule(fl::Rule::parse("if Energy is HIGH and Productivity is MEDIUM and Driven is VERY_LOW then Work is YES", engine));
    rules->addRule(fl::Rule::parse("if Energy is HIGH and Productivity is HIGH and Driven is VERY_HIGH then Work is YES", engine));
    rules->addRule(fl::Rule::parse("if Productivity is MEDIUM and (Driven is HIGH or Driven is VERY_HIGH) then Work is YES", engine));
    rules->addRule(fl::Rule::parse("if Productivity is MEDIUM and (Energy is MEDIUM or Energy is HIGH) and (Driven is LOW or Driven is MODERATE) then Work is YES", engine));
    rules->addRule(fl::Rule::parse("if Productivity is LOW and (Energy is MEDIUM or Energy is HIGH) then Work is YES", engine));
    rules->addRule(fl::Rule::parse("if Productivity is LOW and Energy is LOW and (Driven is MODERATE or Driven is HIGH or Driven is VERY_HIGH) then Work is YES", engine));

    engine->addRuleBlock(rules);
}

void initPlayEngine(fl::Engine *engine)
{
    fl::InputVariable *fun = new fl::InputVariable;
    fun->setName(FUN_INPUT);
    fun->setRange(0.0, 1.0);
    fun->addTerm(new fl::Trapezoid("LOW", 0.0, 0.0, 0.2, 0.35));
    fun->addTerm(new fl::Trapezoid("MEDIUM", 0.3, 0.4, 0.7, 0.75));
    fun->addTerm(new fl::Trapezoid("HIGH", 0.7, 0.75, 1.0, 1.0));
    engine->addInputVariable(fun);

    fl::InputVariable *drivenTrait = new fl::InputVariable;
    drivenTrait->setName(DRIVEN_INPUT);
    drivenTrait->setRange(-1, 1);
    drivenTrait->addTerm(new fl::Trapezoid("VERY_LOW", -1, -1, -0.7, -0.5));
    drivenTrait->addTerm(new fl::Trapezoid("LOW", -0.6, -0.4, 0, 0.1));
    drivenTrait->addTerm(new fl::Trapezoid("MODERATE", 0, 0.1, 0.3, 0.4));
    drivenTrait->addTerm(new fl::Trapezoid("HIGH", 0.3, 0.4, 0.5, 0.6));
    drivenTrait->addTerm(new fl::Trapezoid("VERY_HIGH", 0.5, 0.6, 1, 1));
    engine->addInputVariable(drivenTrait);

    fl::OutputVariable *playGame = new fl::OutputVariable;
    playGame->setName(PLAY_OUTPUT);
    playGame->setRange(0.0, 1.0);
    playGame->setDefaultValue(fl::nan);
    playGame->setAggregation(new fl::AlgebraicSum);
    playGame->setDefuzzifier(new fl::WeightedAverage("Automatic"));
    playGame->addTerm(new fl::Triangle("NO", 0, 0, 0.55));
    playGame->addTerm(new fl::Triangle("YES", 0.45, 1, 1));
    engine->addOutputVariable(playGame);

    fl::RuleBlock *rules = new fl::RuleBlock;
    rules->setName("Work Rules");
    rules->setConjunction(new fl::AlgebraicProduct);
    rules->setDisjunction(new fl::AlgebraicSum);
    rules->addRule(fl::Rule::parse("if Fun is LOW then Play is YES", engine));
    rules->addRule(fl::Rule::parse("if (Fun is MEDIUM or Fun is HIGH) and (Driven is VERY_LOW or Driven is LOW or Driven is MODERATE) then Play is YES", engine));
    rules->addRule(fl::Rule::parse("if Fun is MEDIUM and (Driven is HIGH or Driven is VERY_HIGH) then Play is NO", engine));
    rules->addRule(fl::Rule::parse("if Fun is HIGH and (Driven is HIGH or Driven is VERY_HIGH) then Play is NO", engine));
    engine->addRuleBlock(rules);
}

void initEatEngine(fl::Engine *engine)
{
    fl::InputVariable *hunger = new fl::InputVariable;
    hunger->setName(HUNGER_INPUT);
    hunger->setRange(0.0, 1.0);
    hunger->addTerm(new fl::Trapezoid("LOW", 0.0, 0.0, 0.2, 0.35));
    hunger->addTerm(new fl::Trapezoid("MEDIUM", 0.3, 0.4, 0.7, 0.75));
    hunger->addTerm(new fl::Trapezoid("HIGH", 0.7, 0.75, 1.0, 1.0));
    engine->addInputVariable(hunger);

    fl::InputVariable *energy = new fl::InputVariable;
    energy->setName(ENERGY_INPUT);
    energy->setRange(0.0, 1.0);
    energy->addTerm(new fl::Trapezoid("LOW", 0.0, 0.0, 0.2, 0.35));
    energy->addTerm(new fl::Trapezoid("MEDIUM", 0.3, 0.4, 0.7, 0.75));
    energy->addTerm(new fl::Trapezoid("HIGH", 0.7, 0.75, 1.0, 1.0));
    engine->addInputVariable(energy);

    fl::OutputVariable *eat = new fl::OutputVariable;
    eat->setName(EAT_OUTPUT);
    eat->setRange(0.0, 1.0);
    eat->setDefaultValue(fl::nan);
    eat->setAggregation(new fl::AlgebraicSum);
    eat->setDefuzzifier(new fl::WeightedAverage("Automatic"));
    eat->addTerm(new fl::Triangle("NO", 0, 0, 0.55));
    eat->addTerm(new fl::Triangle("YES", 0.45, 1, 1));
    engine->addOutputVariable(eat);

    fl::RuleBlock *rules = new fl::RuleBlock;
    rules->setName("Coffee Rules");
    rules->setConjunction(new fl::AlgebraicProduct);
    rules->setDisjunction(new fl::AlgebraicSum);
    rules->addRule(fl::Rule::parse("if Hunger is LOW then Eat is YES", engine));
    rules->addRule(fl::Rule::parse("if Hunger is MEDIUM and Energy is LOW then Eat is YES", engine));
    rules->addRule(fl::Rule::parse("if Hunger is MEDIUM and (Energy is HIGH or Energy is MEDIUM) then Eat is NO", engine));
    rules->addRule(fl::Rule::parse("if Hunger is HIGH then Eat is NO", engine));
    engine->addRuleBlock(rules);
}

void initLectureEngine(fl::Engine *engine)
{
    fl::InputVariable *energy = new fl::InputVariable;
    energy->setName(ENERGY_INPUT);
    energy->setRange(0.0, 1.0);
    energy->addTerm(new fl::Trapezoid("LOW", 0.0, 0.0, 0.2, 0.35));
    energy->addTerm(new fl::Trapezoid("MEDIUM", 0.3, 0.4, 0.7, 0.75));
    energy->addTerm(new fl::Trapezoid("HIGH", 0.7, 0.75, 1.0, 1.0));
    engine->addInputVariable(energy);

    fl::InputVariable *fun = new fl::InputVariable;
    fun->setName(FUN_INPUT);
    fun->setRange(0.0, 1.0);
    fun->addTerm(new fl::Trapezoid("LOW", 0.0, 0.0, 0.2, 0.35));
    fun->addTerm(new fl::Trapezoid("MEDIUM", 0.3, 0.4, 0.7, 0.75));
    fun->addTerm(new fl::Trapezoid("HIGH", 0.7, 0.75, 1.0, 1.0));
    engine->addInputVariable(fun);

    fl::InputVariable *drivenTrait = new fl::InputVariable;
    drivenTrait->setName(DRIVEN_INPUT);
    drivenTrait->setRange(-1, 1);
    drivenTrait->addTerm(new fl::Trapezoid("VERY_LOW", -1, -1, -0.7, -0.5));
    drivenTrait->addTerm(new fl::Trapezoid("LOW", -0.6, -0.4, 0, 0.1));
    drivenTrait->addTerm(new fl::Trapezoid("MODERATE", 0, 0.1, 0.3, 0.4));
    drivenTrait->addTerm(new fl::Trapezoid("HIGH", 0.3, 0.4, 0.5, 0.6));
    drivenTrait->addTerm(new fl::Trapezoid("VERY_HIGH", 0.5, 0.6, 1, 1));
    engine->addInputVariable(drivenTrait);

    fl::OutputVariable *attendLecture = new fl::OutputVariable;
    attendLecture->setName(ATTENDLEC_OUTPUT);
    attendLecture->setRange(0.0, 1.0);
    attendLecture->setDefaultValue(fl::nan);
    attendLecture->setAggregation(new fl::AlgebraicSum);
    attendLecture->setDefuzzifier(new fl::WeightedAverage("Automatic"));
    attendLecture->addTerm(new fl::Triangle("NO", 0, 0, 0.55));
    attendLecture->addTerm(new fl::Triangle("YES", 0.45, 1, 1));
    engine->addOutputVariable(attendLecture);

    fl::RuleBlock *rules = new fl::RuleBlock;
    rules->setName("Work Rules");
    rules->setConjunction(new fl::AlgebraicProduct);
    rules->setDisjunction(new fl::AlgebraicSum);
    rules->addRule(fl::Rule::parse("if Driven is HIGH or Driven is VERY_HIGH then AttendLecture is YES", engine));
    rules->addRule(fl::Rule::parse("if Energy is LOW and Fun is MEDIUM and Driven is MODERATE then AttendLecture is YES", engine));
    rules->addRule(fl::Rule::parse("if (Energy is HIGH or Energy is MEDIUM or Fun is HIGH or Fun is MEDIUM) and Driven is MODERATE then AttendLecture is YES", engine));
    rules->addRule(fl::Rule::parse("if (Energy is HIGH or Energy is MEDIUM or Fun is HIGH or (Fun is MEDIUM and (Energy is HIGH or Energy is MEDIUM))) and Driven is LOW then AttendLecture is YES", engine));

    rules->addRule(fl::Rule::parse("if Energy is LOW and Fun is LOW and (Driven is LOW or Driven is MODERATE) then AttendLecture is NO", engine));
    rules->addRule(fl::Rule::parse("if (Energy is LOW or Fun is LOW) and Driven is VERY_LOW then AttendLecture is NO", engine));
    rules->addRule(fl::Rule::parse("if Energy is LOW and Fun is MEDIUM and Driven is LOW then AttendLecture is NO", engine));

    rules->addRule(fl::Rule::parse("if (Energy is HIGH or Energy is MEDIUM) and (Fun is MEDIUM or Fun is HIGH) and Driven is VERY_LOW then AttendLecture is YES", engine));

    engine->addRuleBlock(rules);
}

void initSpawningEngine(fl::Engine *engine)
{
    // University Opening Times
    fl::InputVariable *uniOpeningTime = new fl::InputVariable;
    uniOpeningTime->setName(UNI_OPEN_TIME_INPUT);
    uniOpeningTime->setRange(0, 24);
    uniOpeningTime->addTerm(new fl::Trapezoid("CLOSED_MORNING", 0, 0, 6, 8));
    uniOpeningTime->addTerm(new fl::Trapezoid("OPEN", 7, 8, 18, 20.5));
    uniOpeningTime->addTerm(new fl::Trapezoid("CLOSED_NIGHT", 20, 21, 24, 24));
    engine->addInputVariable(uniOpeningTime);

    // Does NPC have task soon
    fl::InputVariable *nextTask = new fl::InputVariable;
    nextTask->setName(NEXT_TASK_INPUT);
    nextTask->setRange(0, 24); // range represents how soon in hours the task is
    nextTask->addTerm(new fl::Triangle("NOW", 0, 0, 0.5));
    nextTask->addTerm(new fl::Triangle("VERY_SOON", 0.4, 1, 1.75));
    nextTask->addTerm(new fl::Trapezoid("SOON", 1.5, 1.75, 2.5, 3));
    nextTask->addTerm(new fl::Trapezoid("NOT_YET", 2.5, 4, 24, 24));
    engine->addInputVariable(nextTask);

    // Driven Trait
    fl::InputVariable *drivenTrait = new fl::InputVariable;
    drivenTrait->setName(DRIVEN_INPUT);
    drivenTrait->setRange(-1, 1);
    drivenTrait->addTerm(new fl::Trapezoid("VERY_LOW", -1, -1, -0.7, -0.5));
    drivenTrait->addTerm(new fl::Trapezoid("LOW", -0.6, -0.4, 0, 0.1));
    drivenTrait->addTerm(new fl::Trapezoid("MODERATE", 0, 0.1, 0.3, 0.4));
    drivenTrait->addTerm(new fl::Trapezoid("HIGH", 0.3, 0.4, 0.5, 0.6));
    drivenTrait->addTerm(new fl::Trapezoid("VERY_HIGH", 0.5, 0.6, 1, 1));
    engine->addInputVariable(drivenTrait);

    fl::OutputVariable *spawn = new fl::OutputVariable;
    spawn->setName(SPAWN_OUTPUT);
    spawn->setRange(0.0, 1.0);
    spawn->setDefaultValue(fl::nan);
    spawn->setAggregation(new fl::AlgebraicSum);
    spawn->setDefuzzifier(new fl::WeightedAverage("Automatic"));
    spawn->addTerm(new fl::Triangle("NO", 0, 0, 0.55));
    spawn->addTerm(new fl::Triangle("YES", 0.45, 1, 1));
    engine->addOutputVariable(spawn);

    fl::RuleBlock *rules = new fl::RuleBlock;
    rules->setName(SPAWN_OUTPUT);
    rules->setConjunction(new fl::AlgebraicProduct);
    rules->setDisjunction(new fl::AlgebraicSum);
    rules->addRule(fl::Rule::parse("if UniversityOpen is CLOSED_NIGHT or UniversityOpen is CLOSED_MORNING then Spawn is NO", engine));
    rules->addRule(fl::Rule::parse("if UniversityOpen is OPEN and NextTask is NOW then Spawn is YES", engine));
    rules->addRule(fl::Rule::parse("if UniversityOpen is OPEN and NextTask is VERY_SOON and (Driven is MODERATE or Driven is HIGH or Driven is VERY_HIGH) then Spawn is YES", engine));
    rules->addRule(fl::Rule::parse("if UniversityOpen is OPEN and NextTask is VERY_SOON and (Driven is VERY_LOW or Driven is LOW) then Spawn is NO", engine));
    rules->addRule(fl::Rule::parse("if UniversityOpen is OPEN and NextTask is SOON and (Driven is HIGH or Driven is VERY_HIGH) then Spawn is YES", engine));
    rules->addRule(fl::Rule::parse("if UniversityOpen is OPEN and NextTask is SOON and Driven is not HIGH and Driven is not VERY_HIGH then Spawn is NO", engine));
    rules->addRule(fl::Rule::parse("if UniversityOpen is OPEN and NextTask is NOT_YET and Driven is VERY_HIGH then Spawn is YES", engine));
    rules->addRule(fl::Rule::parse("if UniversityOpen is OPEN and NextTask is NOT_YET and Driven is not VERY_HIGH then Spawn is NO", engine));
    engine->addRuleBlock(rules);
}

void initDeactivateNpcEngine(fl::Engine *engine)
{
    // University Opening Times
    fl::InputVariable *uniOpeningTime = new fl::InputVariable;
    uniOpeningTime->setName(UNI_OPEN_TIME_INPUT);
    uniOpeningTime->setRange(0, 24);
    uniOpeningTime->addTerm(new fl::Trapezoid("CLOSED_MORNING", 0, 0, 6, 8));
    uniOpeningTime->addTerm(new fl::Trapezoid("OPEN", 7, 8, 18, 20.5));
    uniOpeningTime->addTerm(new fl::Trapezoid("CLOSED_NIGHT", 20, 21, 24, 24));
    engine->addInputVariable(uniOpeningTime);

    fl::OutputVariable *spawn = new fl::OutputVariable;
    spawn->setName(DISABLE_OUTPUT);
    spawn->setRange(0.0, 1.0);
    spawn->setDefaultValue(fl::nan);
    spawn->setAggregation(new fl::AlgebraicSum);
    spawn->setDefuzzifier(new fl::WeightedAverage("Automatic"));
    spawn->addTerm(new fl::Triangle("NO", 0, 0, 0.55));
    spawn->addTerm(new fl::Triangle("YES", 0.45, 1, 1));
    engine->addOutputVariable(spawn);

    fl::RuleBlock *rules = new fl::RuleBlock;
    rules->setName(SPAWN_OUTPUT);
    rules->setConjunction(new fl::AlgebraicProduct);
    rules->setDisjunction(new fl::AlgebraicSum);
    rules->addRule(fl::Rule::parse("if UniversityOpen is CLOSED_NIGHT or UniversityOpen is CLOSED_MORNING then Disable is YES", engine));
    rules->addRule(fl::Rule::parse("if UniversityOpen is OPEN then Disable is NO", engine));
    // rules->addRule(fl::Rule::parse("if UniversityOpen is OPEN and NextTask is NOT_YET then Disable is YES", engine));
    engine->addRuleBlock(rules);
}