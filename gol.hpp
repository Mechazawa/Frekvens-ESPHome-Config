#pragma once

#ifndef Arduino_h
#include <stdlib.h>
#include <random>

std::random_device rd;
std::mt19937 gen(rd());

int random(int low, int high)
{
    std::uniform_int_distribution<> dist(low, high);
    return dist(gen);
}
#endif

#define MAX_RULES 9

enum EdgeAction
{
  Wrap,
  AlwaysOn,
  AlwaysOff
};

class GameOfLife {
  private:
    char* board;
    unsigned short width;
    unsigned short height;

    // Rules for neighbour count to survive and be born
    unsigned char ruleS[MAX_RULES];
    unsigned char ruleB[MAX_RULES];

    bool hasRule(const unsigned char* rules, const char value) const
    {
      for (int i = 0; i < MAX_RULES; i++) {
        if (rules[i] == value) 
          return true;
      }

      return false;
    }

    void setRules(unsigned char* target, const unsigned char* source, const int count) const 
    {
      this->clearRules(target);

      for (int i = 0; i < count; i++) {
        this->addRule(target, source[i]);
      }
    }
    
    bool addRule(unsigned char* rules, const char value) const
    {
      if (this->hasRule(rules, value)) {
        return true;
      }

      for (int i = 0; i < MAX_RULES; i++) {
        if (rules[i] == 255) {
          rules[i] = value;
          return true;
        }
      }

      return false;
    }

    void clearRules(unsigned char* rules) const
    {
      for (int i = 0; i < MAX_RULES; i++) {
        rules[i] = 255;
      }
    }

  public:
    EdgeAction edgeAction = EdgeAction::Wrap;

    GameOfLife(unsigned short width = 16, unsigned short height = 16) {
      this->width = width;
      this->height = height;
      
      this->board = (char*)malloc(width * height);

      this->setRules(this->ruleS, (const unsigned char[]){2, 3}, 2);
      this->setRules(this->ruleB, (const unsigned char[]){3}, 1);
    }

    ~GameOfLife() {
      free(this->board);
    }

    void tick() {
      for (short x = 0; x < this->width; x++) {
        for (short y = 0; y < this->height; y++) {
          this->tickCell(x, y);
        }
      }
      
      for (int i = 0; i < this->size(); i++) {
        this->board[i] = this->board[i] >> 1;
      }
    }

    void clear() {
	    int cellCount = this->getWidth() * this->getHeight();

      for (int i = 0; i < cellCount; i++) {
        this->board[i] = 0;
      }
    }

    void setRulesS(const unsigned char* rules, const int count) {
      this->setRules(this->ruleS, rules, count);
    }

    void addRuleS(const unsigned char value) {
      this->addRule(this->ruleS, value);
    }

    void clearRuleS() {
      this->clearRules(this->ruleS);
    }

    void setRulesB(const unsigned char* rules, const int count) {
      this->setRules(this->ruleB, rules, count);
    }

    void addRuleB(const unsigned char value) {
      this->addRule(this->ruleB, value);
    }

    void clearRuleB() {
      this->clearRules(this->ruleB);
    }

    void randomize(int chance = 5) {
	  int cellCount = this->getWidth() * this->getHeight();

      for (int i = 0; i < cellCount; i++) {
        this->board[i] = random(0, chance) == 0;
      }
    }

    bool tickCell(short x, short y) {
      char count = this->getNeighborCount(x, y);
      bool alive = this->getCell(x, y);
      
      // alive = count == 3 || (alive && count == 2);

      if (alive && !this->hasRule(this->ruleS, count)){
        alive = false;
      } else if (!alive && this->hasRule(this->ruleB, count)) {
        alive = true;
      }

      if (alive) {
        this->board[x + (y * this->width)] |= 2;
      }

      return alive;
    }

    char getNeighborCount(short x, short y) {
      char count = 0;

      for (short dx = -1; dx <= 1; dx++) {
        for (short dy = -1; dy <= 1; dy++) {
          if (dx != 0 || dy != 0) {
            count += this->getCell(x + dx, y + dy);
          }
        }
      }

      return count;
    }

    bool getCell(short x, short y) {
      // Handle wrapping in both directions
      if (this->edgeAction == EdgeAction::Wrap) {
        x += this->width;
        y += this->height;
        
        x %= this->width;
        y %= this->height;
      }

      if (x < 0 || y < 0 || x >= this->width || y >= this->height) {
        return this->edgeAction == EdgeAction::AlwaysOn;
      }
      
      return (this->board[x + (y * this->width)] & 1) == 1;
    }

    void setCell(short x, short y, bool value) {
	  if (x >= this->getWidth()) return;
	  if (y >= this->getHeight()) return;

      x += this->width;
      y += this->height;
      
      x %= this->width;
      y %= this->height;

      this->board[x + (y * this->width)] = value ? 1 : 0;
    }

    unsigned short getWidth() {
      return this->width;
    }

    unsigned short getHeight() {
      return this->height;
    }

    unsigned short size() {
      return this->height * this->width;
    }
};

// todo use global in esphome
GameOfLife gol;
